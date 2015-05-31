#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/locks.hpp> 

using namespace boost;
using namespace std;

#include "vae_db_handler.h"
#include "site.h"
#include "context.h"
#include "logger.h"
#include "reaper.h"
#include "response.h"
#include "session.h"
#include "s3.h"

void eatErrors(void * ctx, const char * msg, ...) { }

boost::shared_ptr<Site> VaeDbHandler::getSite(string subdomain, string secretKey, bool stagingMode) {
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
boost::mutex & VaeDbHandler::_get_site_mutex(std::string const & subdomain, bool stagingMode) {
  string sitesKey(stagingMode ? subdomain + ".staging" : subdomain);
  boost::unique_lock<boost::mutex> lockSites(sitesMutex);  
  if (siteMutexes.count(sitesKey)) {
    return *siteMutexes[sitesKey];
  } else {
    return *(siteMutexes[sitesKey] = new boost::mutex);
  }
}

inline boost::shared_ptr<class Site>
VaeDbHandler::_loadSite(string const & subdomain, bool stagingMode, string const & xml) {
  boost::shared_ptr<Site> site;
  site.reset(new Site(subdomain, stagingMode, xml));
  boost::unique_lock<boost::mutex> lockSites(sitesMutex);
  string sitesKey(stagingMode ? subdomain + ".staging" : subdomain);
  return sites[sitesKey] = site;
}
 
boost::shared_ptr<Site> VaeDbHandler::_getSite(string const & sitesKey, string const & secretKey) {
  boost::unique_lock<boost::mutex> lock(sitesMutex);

  if (sites.count(sitesKey)) {
    sites[sitesKey]->validateSecretKey(secretKey);
    return sites[sitesKey];
  }

  return boost::shared_ptr<Site>();
}

VaeDbHandler::VaeDbHandler(QueryLog &queryLog, MemcacheProxy &memcacheProxy) 
  :  queryLog(queryLog), memcacheProxy(memcacheProxy) {

  nextSessionId = 1;
  xmlInitParser();
  xmlSetGenericErrorFunc(NULL, eatErrors);
  writePid();
  new Reaper(this);  
  L(info) << "VaeDB Running";
}

VaeDbHandler::~VaeDbHandler() { 
  L(info) << "shutdown";
  xmlCleanupParser();
}

void VaeDbHandler::closeSession(const int32_t sessionId, const string& secretKey) {
  QueryLogEntry entry(queryLog);
  entry.method_call("closeSession") << sessionId << secretKey << "\n";

  boost::unique_lock<boost::mutex> lock(sessionsMutex);
  if (sessions.count(sessionId)) {
    sessions.erase(sessionId);
  } else {
    L(warning) << "closeSession() called with an invalid session ID: " << sessionId;
  }
}

void VaeDbHandler::createInfo(VaeDbCreateInfoResponse& _return, const int32_t sessionId, const int32_t responseId, const string& query) {
  QueryLogEntry entry(queryLog);
  entry.method_call("createInfo") << sessionId << responseId << query << "\n";

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
  session->createInfo(_return, responseId, query);
}

void VaeDbHandler::data(VaeDbDataResponse& _return, const int32_t sessionId, const int32_t responseId) {
  QueryLogEntry entry(queryLog);
  entry.method_call("data") << sessionId << responseId << "\n";

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
  
void VaeDbHandler::get(VaeDbResponse& _return, const int32_t sessionId, const int32_t responseId, const string& query, const map<string, string> & options) {
  QueryLogEntry entry(queryLog);
  entry.method_call("get") << sessionId << responseId << query << options << "\n";

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
  
SessionMap& VaeDbHandler::getSessions() {
  return sessions;
}

int32_t VaeDbHandler::openSession(const string& subdomain, const string& secretKey, const bool stagingMode, const int32_t suggestedSessionId) {
  QueryLogEntry entry(queryLog);
  entry.method_call("openSession") << subdomain << secretKey << stagingMode << suggestedSessionId << "\n";

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
  }
  return sessionId;
}

int8_t VaeDbHandler::ping() {
  QueryLogEntry entry(queryLog);
  entry.method_call("ping") << "\n";

  L(info) << "[ping]";
  return 0;
}

void VaeDbHandler::resetSite(string const & subdomain, string const & secretKey) {
  QueryLogEntry entry(queryLog);
  entry.method_call("resetSite") << subdomain << secretKey << "\n";
  _resetSite(subdomain, secretKey, false);
}

void VaeDbHandler::reloadSite(string const & subdomain) {
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
void VaeDbHandler::_resetSite(string const & subdomain, string const & secretKey, bool force) {
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
void VaeDbHandler::_eraseSite(string const & sitesKey, string const & secretKey, bool force) {
  //expects siteMutex held

  if(!force)
    sites[sitesKey]->validateSecretKey(secretKey);

  sites.erase(sitesKey);
}

void VaeDbHandler::structure(VaeDbStructureResponse& _return, const int32_t sessionId, const int32_t responseId) {
  QueryLogEntry entry(queryLog);
  entry.method_call("structure") << sessionId << responseId << "\n";

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

void VaeDbHandler::writePid() {
  ofstream pidfile;
  pidfile.open("/tmp/vaedb.pid");
  if (pidfile.is_open()) {
    pidfile << getpid();
  } else {
    L(warning) << "Could not write PID file /tmp/vaedb.pid";
  }
  pidfile.close();
}

void VaeDbHandler::shortTermCacheGet(string &_return, const int32_t sessionId, string const & key, const int32_t flags) {
  QueryLogEntry entry(queryLog);
  entry.method_call("shortTermCacheGet") << sessionId << key << "\n";

  boost::shared_ptr<class Session> session;
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    if (sessions.count(sessionId)) {
      session = sessions[sessionId];
    } else {
      L(warning) << "shortTermCacheGet() called with an invalid session ID: " << sessionId;
      return;
    }
  }
  string fullKey = "VaedbProxy:" + session->getSite()->getSubdomain() + ":" + key;
  _return = memcacheProxy.get(fullKey, flags);
}

void VaeDbHandler::shortTermCacheSet(const int32_t sessionId, string const & key, string const & value, const int32_t flags, const int32_t expireInterval) {
  QueryLogEntry entry(queryLog);
  entry.method_call("shortTermCacheSet") << sessionId << key << "\n";

  boost::shared_ptr<class Session> session;
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    if (sessions.count(sessionId)) {
      session = sessions[sessionId];
    } else {
      L(warning) << "shortTermCacheSet() called with an invalid session ID: " << sessionId;
      return;
    }
  }
  string fullKey = "VaedbProxy:" + session->getSite()->getSubdomain() + ":" + key;
  memcacheProxy.set(fullKey, value, flags, expireInterval);
}

void VaeDbHandler::longTermCacheGet(string &_return, const int32_t sessionId, string const & key, const int32_t renewExpiry) {
  QueryLogEntry entry(queryLog);
  entry.method_call("longTermCacheGet") << sessionId << key << "\n";

  _return = "Long Term Value";
}

void VaeDbHandler::longTermCacheSet(const int32_t sessionId, string const & key, string const & value, const int32_t expireInterval, const int32_t isFilename) {
  QueryLogEntry entry(queryLog);
  entry.method_call("longTermCacheSet") << sessionId << key << "\n";

}

void VaeDbHandler::longTermCacheEmpty(const int32_t sessionId) {
  QueryLogEntry entry(queryLog);
  entry.method_call("longTermCacheEmpty") << sessionId << "\n";

}
