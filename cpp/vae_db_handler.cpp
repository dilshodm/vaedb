#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/locks.hpp> 

using namespace boost;
using namespace std;

#include "../gen-cpp/VaeDb.h"
#include "site.h"
#include "context.h"
#include "logger.h"
#include "reaper.h"
#include "response.h"
#include "session.h"
#include "vae_db_handler.h"

void eatErrors(void * ctx, const char * msg, ...) { }

shared_ptr<Site> VaeDbHandler::getSite(string subdomain, string secretKey, bool stagingMode) {
  boost::mutex *siteMutex;
  time_t modTime;
  string sitesKey;
  sitesKey = (stagingMode ? subdomain + ".staging" : subdomain);
  {
    boost::unique_lock<boost::mutex> lock(sitesMutex);
    if (sites.count(sitesKey)) {
      sites[sitesKey]->validateSecretKey(secretKey);
      filesystem::path p(sites[sitesKey]->filename);
      modTime = filesystem::last_write_time(p);
      if (modTime == sites[sitesKey]->modTime) {
        return sites[sitesKey];
      }
    }
    if (siteMutexes.count(sitesKey)) {
      siteMutex = siteMutexes[sitesKey];
    } else {
      siteMutex = new boost::mutex;
      siteMutexes[sitesKey] = siteMutex;
    }
  }
  {
    boost::unique_lock<boost::mutex> lock(*siteMutex);
    {
      boost::unique_lock<boost::mutex> lock(sitesMutex);
      if (sites.count(sitesKey)) {
        sites[sitesKey]->validateSecretKey(secretKey);
        filesystem::path p(sites[sitesKey]->filename);
        modTime = filesystem::last_write_time(p);
        if (modTime == sites[sitesKey]->modTime) {
          return sites[sitesKey];
        }
      }
    }
    shared_ptr<Site> site(new Site(subdomain, secretKey, testMode, stagingMode));
    {
      boost::unique_lock<boost::mutex> lock(sitesMutex);
      sites[sitesKey] = site;
    }
    return site;
  }
}
 
VaeDbHandler::VaeDbHandler(bool t) : testMode(t) {
  nextSessionId = 1;
  xmlInitParser();
  xmlSetGenericErrorFunc(NULL, eatErrors);
  writePid();
  new Reaper(this);  
  L(info) << "VaeDB Running (" << (testMode ? "TEST" : "production") << " environment)";
}

VaeDbHandler::~VaeDbHandler() { 
  L(info) << "shutdown";
  xmlCleanupParser();
}

void VaeDbHandler::closeSession(const int32_t sessionId, const string& secretKey) {
  boost::unique_lock<boost::mutex> lock(sessionsMutex);
  if (sessions.count(sessionId)) {
    sessions.erase(sessionId);
  } else {
    L(warning) << "closeSession() called with an invalid session ID: " << sessionId;
  }
}

void VaeDbHandler::createInfo(VaeDbCreateInfoResponse& _return, const int32_t sessionId, const int32_t responseId, const string& query) {
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
  session->createInfo(_return, responseId, query);
}

void VaeDbHandler::data(VaeDbDataResponse& _return, const int32_t sessionId, const int32_t responseId) {
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
  session->data(_return, responseId);
}
  
void VaeDbHandler::get(VaeDbResponse& _return, const int32_t sessionId, const int32_t responseId, const string& query, const map<string, string> & options) {
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
  session->get(_return, responseId, query, options);
}
  
SessionMap& VaeDbHandler::getSessions() {
  return sessions;
}

int32_t VaeDbHandler::openSession(const string& subdomain, const string& secretKey, const bool stagingMode) {
  int32_t sessionId; 
  shared_ptr<Site> site = getSite(subdomain, secretKey, stagingMode);
  {
    boost::unique_lock<boost::mutex> lock(sessionsMutex);
    do { 
      sessionId = rand();
    } while (sessions.count(sessionId));
    shared_ptr<Session> session(new Session(site));
    sessions[sessionId] = session;
  }
  return sessionId;
}

int8_t VaeDbHandler::ping() {
  L(info) << "[ping]";
  return 0;
}

void VaeDbHandler::resetSite(const string& subdomain, const std::string& secretKey) {
  boost::unique_lock<boost::mutex> lock(sitesMutex);
  if (sites.count(subdomain)) {
    sites[subdomain]->validateSecretKey(secretKey);
    sites.erase(subdomain);
  }
  string staging = subdomain + ".staging";
  if (sites.count(staging)) {
    sites[staging]->validateSecretKey(secretKey);
    sites.erase(staging);
  }
}

void VaeDbHandler::structure(VaeDbStructureResponse& _return, const int32_t sessionId, const int32_t responseId) {
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
