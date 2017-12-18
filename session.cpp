#include <iostream>
#include <time.h>
#include <boost/smart_ptr.hpp>

using namespace std;
using namespace boost;

#include "site.h"
#include "context.h"
#include "logger.h"
#include "response.h"
#include "session.h"

Session::Session(boost::shared_ptr<Site> s) {
  site = s;
  createdAt = time(NULL);
}

json Session::createInfo(const int32_t responseId, const string& query) {
  boost::shared_ptr<Response> parent;
  L(info) << "[" << site->getSubdomain() << "] createInfo '" << query << "'";
  if (responseId) {
    if (responses.size() >= responseId) {
      parent = responses[responseId-1];
    } else {
      throw VaeDbInternalError("Invalid responseId");
    }
  } else {
    parent = boost::shared_ptr<Response>();
  }
  boost::shared_ptr<Response> response(new Response(site, parent, query));
  return response->getCreateInfo();
}

json Session::data(const int32_t responseId) {
  boost::shared_ptr<Response> response;
  L(info) << "[" << site->getSubdomain() << "] data";
  if (responses.size() >= responseId) {
    response = responses[responseId-1];
  } else {
    throw VaeDbInternalError("Invalid responseId");
  }
  return response->getData();
}

json Session::get(const int32_t responseId, const std::string& query, const std::map<std::string, std::string> & options) {
  boost::shared_ptr<Response> parent;
  L(info) << "[" << site->getSubdomain() << "] get '" << query << "'";
  if (responseId) {
    if (responses.size() >= responseId) {
      parent = responses[responseId-1];
    } else {
      throw VaeDbInternalError("Invalid responseId");
    }
  } else {
    parent = boost::shared_ptr<Response>();
  }
  boost::shared_ptr<Response> response(new Response(site, parent, query, options));
  json res = response->getJson();
  res["id"] = responses.size()+1;
  responses.push_back(response);
  return res;
}

time_t Session::getCreatedAt() {
  return createdAt;
}

boost::shared_ptr<Site> Session::getSite() {
  return site;
}

json Session::structure(const int32_t responseId) {
  boost::shared_ptr<Response> response;
  L(info) << "[" << site->getSubdomain() << "] structure";
  if (responses.size() >= responseId) {
    response = responses[responseId-1];
  } else {
    throw VaeDbInternalError("Invalid responseId");
  }
  return response->getStructure();
}
