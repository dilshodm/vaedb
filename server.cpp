#include <iostream>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/locks.hpp>

using namespace boost;
using namespace std;

#include "server.h"
#include "site.h"
#include "context.h"
#include "logger.h"
#include "reaper.h"
#include "response.h"
#include "session.h"
#include "s3.h"

void eatErrors(void * ctx, const char * msg, ...) { }

Server::Server(int workers, int port, QueryLog &queryLog, MemcacheProxy &memcacheProxy, MysqlProxy &mysqlProxy)
  :  queryLog(queryLog), memcacheProxy(memcacheProxy), mysqlProxy(mysqlProxy) {

  nextSessionId = 1;
  xmlInitParser();
  xmlSetGenericErrorFunc(NULL, eatErrors);
  writePid();
  new Reaper(this, mysqlProxy);

  served::multiplexer mux;
  map <string, boost::function<json(boost::shared_ptr<class Session>, string)>> nonSessionEndpoints = {
    { "/ping",                 boost::bind(&Server::ping,                 this, _2) },
  };
  map <string, boost::function<json(boost::shared_ptr<class Session>, string)>> sessionEndpoints = {
    { "/closeSession"     ,    boost::bind(&Server::shortTermCacheGet,    this, _1, _2) },
    { "/shortTermCacheGet",    boost::bind(&Server::shortTermCacheGet,    this, _1, _2) },
    { "/sndlhortTermCacheSet", boost::bind(&Server::shortTermCacheSet,    this, _1, _2) },
    { "/shortTermCacheDelete", boost::bind(&Server::shortTermCacheDelete, this, _1, _2) }
  };
  for (auto const& it : sessionEndpoints) {
    mux.handle(it.first).get([it, this](served::response &res, const served::request &req) {
      process(it.first, true, req, res, it.second);
    }).post([it, this](served::response &res, const served::request &req) {
      process(it.first, true, req, res, it.second);
    });
  }
  for (auto const& it : nonSessionEndpoints) {
    mux.handle(it.first).get([it, this](served::response &res, const served::request &req) {
      process(it.first, false, req, res, it.second);
    }).post([it, this](served::response &res, const served::request &req) {
      process(it.first, false, req, res, it.second);
    });
  }
  served::net::server server("127.0.0.1", to_string(port), mux);
  server.run(workers);

  L(error) << "VaeDB Running";
}

Server::~Server() {
  L(info) << "shutdown";
  xmlCleanupParser();
}

void Server::process(string endpoint, bool requiresSession, const served::request &req, served::response &res, boost::function<json(boost::shared_ptr<class Session>, json)> func) {
  try {
    res.set_header("Content-type", "application/json");
    res.set_header("Server", "vaedb");
    L(info) << "[" << endpoint << "]";
    string body = req.body();
    json params = body.length() > 0 ? json::parse(req.body()) : json::parse("{}");
    QueryLogEntry entry(queryLog);
    entry.method_call(endpoint);

    boost::shared_ptr<class Session> session = NULL;
    if (requiresSession) {
      if (params["sessionId"].is_null()) {
        L(warning) << "[" << endpoint << "] called with missing session ID";
        res.set_status(400);
        res << "{error:'missing session ID'}";
        return;
      }
      int32_t sessionId = params["sessionId"].get<int32_t>();
      {
        boost::unique_lock<boost::mutex> lock(sessionsMutex);
        if (sessions.count(sessionId)) {
          session = sessions[sessionId];
        } else {
          L(warning) << "[" << endpoint << "] called with an invalid session ID: " << sessionId;
          res.set_status(400);
          res << "{error:'invalid session ID'}";
          return;
        }
      }
    }

    L(debug) << "  invoking handler";
    json jsonRes = func(session, params);
    if (!jsonRes.is_null()) res << jsonRes.dump();
  } catch (const std::exception& ex) {
    res.set_status(500);
    res << "{error:'error'}";
    L(error) << "[" << endpoint << "] Error: " << ex.what();
  } catch (...) {
    res.set_status(500);
    res << "{error:'error'}";
    L(error) << "[" << endpoint << "] Unknown exception occurred in handler.";
  }
}

boost::shared_ptr<Site> Server::getSite(string subdomain, string secretKey, bool stagingMode) {
  string xml("");
  string sitesKey(stagingMode ? subdomain + ".staging" : subdomain);
  boost::unique_lock<boost::mutex> lockSite(_get_site_mutex(subdomain, stagingMode));

  boost::shared_ptr<Site> site(_getSite(sitesKey, secretKey));

  if (site) return site;

  if (!testMode) {
    xml = read_s3(subdomain+"-feed.xml");
  }
  site = _loadSite(subdomain, stagingMode, xml);
  site->validateSecretKey(secretKey);
  return site;
}

inline
boost::mutex & Server::_get_site_mutex(std::string const & subdomain, bool stagingMode) {
  string sitesKey(stagingMode ? subdomain + ".staging" : subdomain);
  boost::unique_lock<boost::mutex> lockSites(sitesMutex);
  if (siteMutexes.count(sitesKey)) {
    return *siteMutexes[sitesKey];
  } else {
    return *(siteMutexes[sitesKey] = new boost::mutex);
  }
}

inline boost::shared_ptr<class Site>
Server::_loadSite(string const & subdomain, bool stagingMode, string const & xml) {
  boost::shared_ptr<Site> site;
  site.reset(new Site(subdomain, stagingMode, xml));
  boost::unique_lock<boost::mutex> lockSites(sitesMutex);
  string sitesKey(stagingMode ? subdomain + ".staging" : subdomain);
  return sites[sitesKey] = site;
}

boost::shared_ptr<Site> Server::_getSite(string const & sitesKey, string const & secretKey) {
  boost::unique_lock<boost::mutex> lock(sitesMutex);

  if (sites.count(sitesKey)) {
    sites[sitesKey]->validateSecretKey(secretKey);
    return sites[sitesKey];
  }

  return boost::shared_ptr<Site>();
}

void Server::reloadSite(string const & subdomain) {
  bool reload_prod;
  bool reload_staging;

  {
    boost::unique_lock<boost::mutex> lock(sitesMutex);
    reload_prod = sites.count(subdomain) > 0;
    reload_staging = sites.count(subdomain + ".staging") > 0;
  }

  string rawxml(read_s3(subdomain+"-feed.xml"));
  _resetSite(subdomain, "", true);


  if (reload_prod) {
     boost::unique_lock<boost::mutex> lockSite(_get_site_mutex(subdomain, 0));
    _loadSite(subdomain, 0, rawxml);
  }

  if (reload_staging) {
     boost::unique_lock<boost::mutex> lockSite(_get_site_mutex(subdomain, 1));
    _loadSite(subdomain, 1, rawxml);
  }
}

inline
void Server::_resetSite(string const & subdomain, string const & secretKey, bool force) {
  boost::unique_lock<boost::mutex> lock(sitesMutex);
  if (sites.count(subdomain)) {
    _eraseSite(subdomain, secretKey, force);
  }

  string staging(subdomain + ".staging");

  if (sites.count(staging)) {
    _eraseSite(staging, secretKey, force);
  }
}

inline
void Server::_eraseSite(string const & sitesKey, string const & secretKey, bool force) {
  //expects siteMutex held

  if(!force)
    sites[sitesKey]->validateSecretKey(secretKey);

  sites.erase(sitesKey);
}

SessionMap& Server::getSessions() {
  return sessions;
}

void Server::writePid() {
  ofstream pidfile;
  pidfile.open("/tmp/vaedb.pid");
  if (pidfile.is_open()) {
    pidfile << getpid();
  } else {
    L(warning) << "Could not write PID file /tmp/vaedb.pid";
  }
  pidfile.close();
}


// Handlers


json Server::ping(json params) {
  return json({ { "ping", "pong" } });
}

json Server::closeSession(boost::shared_ptr<class Session>, json params) {
  int32_t sessionId = params["sessionId"].get<int32_t>();

  boost::unique_lock<boost::mutex> lock(sessionsMutex);
  if (sessions.count(sessionId)) {
    sessions.erase(sessionId);
  } else {
    L(warning) << "closeSession() called with an invalid session ID: " << sessionId;
  }
  return json( { { "session", sessionId } });
}

/*
json Server::createInfo(const int32_t sessionId, const int32_t responseId, const string& query) {
  boost::shared_ptr<class Session> session;
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    if (sessions.count(sessionId)) {
      session = sessions[sessionId];
    } else {
      L(warning) << "createInfo() called with an invalid session ID: " << sessionId;
      return;
    }
  }
  entry.set_subdomain(session->getSite()->getSubdomain());
  return session->createInfo(_return, responseId, query);
}

void Server::data(VaeDbDataResponse& _return, const int32_t sessionId, const int32_t responseId) {
  boost::shared_ptr<class Session> session;
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    if (sessions.count(sessionId)) {
      session = sessions[sessionId];
    } else {
      L(warning) << "data() called with an invalid session ID: " << sessionId;
      return;
    }
  }
  entry.set_subdomain(session->getSite()->getSubdomain());
  session->data(_return, responseId);
}

void Server::get(VaeDbResponse& _return, const int32_t sessionId, const int32_t responseId, const string& query, const map<string, string> & options) {
  boost::shared_ptr<class Session> session;
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    if (sessions.count(sessionId)) {
      session = sessions[sessionId];
    } else {
      L(warning) << "get() called with an invalid session ID: " << sessionId;
      return;
    }
  }
  entry.set_subdomain(session->getSite()->getSubdomain());
  session->get(_return, responseId, query, options);
}

void Server::openSession(VaeDbOpenSessionResponse& _return, const string& subdomain, const string& secretKey, const bool stagingMode, const int32_t suggestedSessionId) {
  int32_t sessionId;
  sessionId = suggestedSessionId;
  boost::shared_ptr<Site> site = getSite(subdomain, secretKey, stagingMode);
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    while (sessions.count(sessionId)) {
      sessionId = rand();
    }
    boost::shared_ptr<Session> session(new Session(site));
    sessions[sessionId] = session;
    entry.set_subdomain(session->getSite()->getSubdomain());
    _return.generation = session->getSite()->getGeneration();
  }

  _return.session_id = sessionId;
}

void Server::structure(VaeDbStructureResponse& _return, const int32_t sessionId, const int32_t responseId) {
  boost::shared_ptr<class Session> session;
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    if (sessions.count(sessionId)) {
      session = sessions[sessionId];
    } else {
      L(warning) << "structure() called with an invalid session ID: " << sessionId;
      return;
    }
  }
  entry.set_subdomain(session->getSite()->getSubdomain());
  session->structure(_return, responseId);
}
*/

string Server::longTermKey(boost::shared_ptr<class Session>session, string key) {
  return "VaedbProxy:" + session->getSite()->getSubdomain() + "LongTerm2:" + key;
}

string Server::shortTermKey(boost::shared_ptr<class Session>session, string key) {
  return "VaedbProxy:" + session->getSite()->getSubdomain() + ":" + key;
}

json Server::shortTermCacheGet(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();
  int32_t flags = params["flags"].get<int32_t>();

  return json( { { "key", key }, { "value", memcacheProxy.get(shortTermKey(session, key), flags) } });
}

json Server::shortTermCacheSet(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();
  string value = params["value"].get<string>();
  int32_t flags = params["flags"].get<int32_t>();
  int32_t expireInterval = params["expireInterval"].get<int32_t>();

  memcacheProxy.set(shortTermKey(session, key), value, flags, expireInterval);
  return json( { { "key", key }, { "value", value } });
}

json Server::shortTermCacheDelete(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();

  memcacheProxy.remove(shortTermKey(session, key));
  return json({ { "key", key } });
}

json Server::sessionCacheGet(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();

  return json( { { "key", key }, { "value", mysqlProxy.sessionCacheGet(session->getSite()->getSubdomain(), key) } });
}

json Server::sessionCacheSet(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();
  string value = params["value"].get<string>();

  mysqlProxy.sessionCacheSet(session->getSite()->getSubdomain(), key, value);
  return json( { { "key", key }, { "value", value } });
}

json Server::sessionCacheDelete(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();

  mysqlProxy.sessionCacheDelete(session->getSite()->getSubdomain(), key);
  return json( { { "key", key } });
}

json Server::longTermCacheGet(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();
  int32_t renewExpiry = params["renewExpiry"].get<int32_t>();
  int32_t useShortTermCache = params["useShortTermCache"].get<int32_t>();

  if (useShortTermCache) {
    string answer(memcacheProxy.get(longTermKey(session, key), 0));
    if (answer.length() > 0) {
      return json( { { "key", key }, { "value", answer } });
    }
  }
  string value = mysqlProxy.longTermCacheGet(session->getSite()->getSubdomain(), key, renewExpiry);
  memcacheProxy.set(longTermKey(session, key), value, 0, 86400);
  return json( { { "key", key }, { "value", value } });
}

json Server::longTermCacheSet(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();
  string value = params["value"].get<string>();
  int32_t expireInterval = params["expireInterval"].get<int32_t>();
  int32_t isFilename = params["isFilename"].get<int32_t>();

  mysqlProxy.longTermCacheSet(session->getSite()->getSubdomain(), key, value, expireInterval, isFilename);
  memcacheProxy.set(longTermKey(session, key), value, 0, 86400);
  return json( { { "key", key }, { "value", value } });
}

json Server::longTermCacheDelete(boost::shared_ptr<class Session> session, json params) {
  string key = params["key"].get<string>();

  mysqlProxy.longTermCacheDelete(session->getSite()->getSubdomain(), key);
  memcacheProxy.remove(longTermKey(session, key));
  return json( { { "key", key } });
}

json Server::longTermCacheEmpty(boost::shared_ptr<class Session> session, json params) {
  mysqlProxy.longTermCacheEmpty(session->getSite()->getSubdomain());
  return json( { { "empty", true } });
}

json Server::longTermCacheSweeperInfo(boost::shared_ptr<class Session> session, json params) {
  return mysqlProxy.longTermCacheSweeperInfo(session->getSite()->getSubdomain());
}

json Server::sitewideLock(boost::shared_ptr<class Session> session, json params) {
  string iden = params["iden"].get<string>();

  string fullKey = session->getSite()->getSubdomain() + ":" + iden;
  return { { "status", mysqlProxy.lock(fullKey) } };
}

json Server::sitewideUnlock(boost::shared_ptr<class Session> session, json params) {
  string iden = params["iden"].get<string>();

  string fullKey = session->getSite()->getSubdomain() + ":" + iden;
  return { { "status", mysqlProxy.unlock(fullKey) } };
}
