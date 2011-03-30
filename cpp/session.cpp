#include <iostream>
#include <time.h>
#include <boost/smart_ptr.hpp>

using namespace std;
using namespace boost;

#include "../gen-cpp/VerbDb.h"
#include "site.h"
#include "context.h"
#include "logger.h"
#include "response.h"
#include "session.h"

Session::Session(shared_ptr<Site> s) { 
  site = s;
  createdAt = time(NULL);
}

void Session::createInfo(VerbDbCreateInfoResponse& _return, const int32_t responseId, const string& query) {
  shared_ptr<Response> parent;
  L(info) << "[" << site->getSubdomain() << "] createInfo '" << query << "'";
  if (responseId) {
    if (responses.size() >= responseId) {
      parent = responses[responseId-1];
    } else {
      throw VerbDbInternalError("Invalid responseId");
    }
  } else {
    parent = shared_ptr<Response>();
  } 
  shared_ptr<Response> response(new Response(site, parent, query));
  response->writeVerbDbCreateInfoResponse(_return);
}

void Session::data(VerbDbDataResponse& _return, const int32_t responseId) {
  shared_ptr<Response> response;
  L(info) << "[" << site->getSubdomain() << "] data";
  if (responses.size() >= responseId) {
    response = responses[responseId-1];
  } else {
    throw VerbDbInternalError("Invalid responseId");
  }
  response->writeVerbDbDataResponse(_return);
}

void Session::get(VerbDbResponse& _return, const int32_t responseId, const std::string& query, const std::map<std::string, std::string> & options) {
  shared_ptr<Response> parent;
  L(info) << "[" << site->getSubdomain() << "] get '" << query << "'";
  if (responseId) {
    if (responses.size() >= responseId) {
      parent = responses[responseId-1];
    } else {
      throw VerbDbInternalError("Invalid responseId");
    }
  } else {
    parent = shared_ptr<Response>();
  } 
  shared_ptr<Response> response(new Response(site, parent, query, options));
  response->writeVerbDbResponse(_return);
  _return.id = responses.size()+1;
  responses.push_back(response);
}

time_t Session::getCreatedAt() {
  return createdAt;
}

shared_ptr<Site> Session::getSite() {
  return site;
}

void Session::structure(VerbDbStructureResponse& _return, const int32_t responseId) {
  shared_ptr<Response> response;
  L(info) << "[" << site->getSubdomain() << "] structure";
  if (responses.size() >= responseId) {
    response = responses[responseId-1];
  } else {
    throw VerbDbInternalError("Invalid responseId");
  }
  response->writeVerbDbStructureResponse(_return);
}
