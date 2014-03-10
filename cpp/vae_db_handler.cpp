#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/locks.hpp> 

using namespace boost;
using namespace std;

#include "site.h"
#include "context.h"
#include "logger.h"
#include "reaper.h"
#include "response.h"
#include "session.h"
#include "s3.h"
#include "vae_db_handler.h"

void eatErrors(void * ctx, const char * msg, ...) { }

shared_ptr<Site> VaeDbHandler::getSite(string subdomain, string secretKey, bool stagingMode) {
  boost::mutex *siteMutex;
  string sitesKey(stagingMode ? subdomain + ".staging" : subdomain);
  shared_ptr<Site> site(_getSite(sitesKey, secretKey));

  if(site)
      return site;
  
  string feedfile(subdomain+"-feed.xml");
  string xml(read_s3(feedfile));
  site = _loadSite(subdomain, stagingMode, xml);
  site->validateSecretKey(secretKey);
  return site;
}

inline boost::shared_ptr<class Site>
VaeDbHandler::_loadSite(string const & subdomain, bool stagingMode, string const & xml) {
  boost::mutex *siteMutex;
  string sitesKey(stagingMode ? subdomain + ".staging" : subdomain);
  shared_ptr<Site> site;

  {
    boost::unique_lock<boost::mutex> lockSites(sitesMutex);
    if(siteMutexes.count(sitesKey))
      siteMutex = siteMutexes[sitesKey];
    else 
      siteMutexes[sitesKey] = siteMutex = new boost::mutex;     
  }

  {
    boost::unique_lock<boost::mutex> lockSite(*siteMutex);
    site.reset(new Site(subdomain, stagingMode, xml));

    boost::unique_lock<boost::mutex> lockSites(sitesMutex);
    sites[sitesKey] = site;
  }

  return site;
}
 
shared_ptr<Site> VaeDbHandler::_getSite(string const & sitesKey, string const & secretKey) {
  boost::unique_lock<boost::mutex> lock(sitesMutex);

  if(sites.count(sitesKey)) {
    sites[sitesKey]->validateSecretKey(secretKey);
    return sites[sitesKey];
  }

  return shared_ptr<Site>();
}

VaeDbHandler::VaeDbHandler(QueryLog & queryLog) 
  :  queryLog(queryLog) {

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
  entry.method_call("closeSession") << sessionId << secretKey << end;

  boost::unique_lock<boost::mutex> lock(sessionsMutex);
  if (sessions.count(sessionId)) {
    sessions.erase(sessionId);
  } else {
    L(warning) << "closeSession() called with an invalid session ID: " << sessionId;
  }
}

void VaeDbHandler::createInfo(VaeDbCreateInfoResponse& _return, const int32_t sessionId, const int32_t responseId, const string& query) {
  QueryLogEntry entry(queryLog);
  entry.method_call("createInfo") << sessionId << responseId << query << end;

  shared_ptr<class Session> session;
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
  entry.method_call("data") << sessionId << responseId << end;

  shared_ptr<class Session> session;
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
  entry.method_call("get") << sessionId << responseId << query << options << end;

  shared_ptr<class Session> session;
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
  entry.method_call("openSession") << subdomain << secretKey << stagingMode << suggestedSessionId << end;

  int32_t sessionId; 
  sessionId = suggestedSessionId;
  shared_ptr<Site> site = getSite(subdomain, secretKey, stagingMode);
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    while (sessions.count(sessionId)) {
      sessionId = rand();
    }
    shared_ptr<Session> session(new Session(site));
    sessions[sessionId] = session;
    entry.set_subdomain(session->getSite()->getSubdomain());
  }
  return sessionId;
}

int8_t VaeDbHandler::ping() {
  QueryLogEntry entry(queryLog);
  entry.method_call("ping") << end;

  L(info) << "[ping]";
  return 0;
}

void VaeDbHandler::resetSite(string const & subdomain, string const & secretKey) {
  QueryLogEntry entry(queryLog);
  entry.method_call("resetSite") << subdomain << secretKey << end;
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

  if(reload_prod)
    _loadSite(subdomain, 0, rawxml);

  if(reload_staging)
    _loadSite(subdomain, 1, rawxml);
}

inline
void VaeDbHandler::_resetSite(string const & subdomain, string const & secretKey, bool force) {
  boost::unique_lock<boost::mutex> lock(sitesMutex);
  if(sites.count(subdomain)) 
    _eraseSite(subdomain, secretKey, force);

  string staging(subdomain + ".staging");
  if(sites.count(staging))
    _eraseSite(staging, secretKey, force); 
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
  entry.method_call("structure") << sessionId << responseId << end;

  shared_ptr<class Session> session;
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
