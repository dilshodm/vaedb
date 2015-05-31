#include <string>
#include <pcrecpp.h>

#include "logger.h"
#include "memcache_proxy.h"

using std::string;

MemcacheProxy::MemcacheProxy(std::string connectString) {
  pcrecpp::RE(",").Replace(" --SERVER=", &connectString);
  connectString = "--SERVER=" + connectString;
  L(info) << " Connecting to Memcache via: " << connectString;
  client = new memcache::Memcache(connectString);
}

MemcacheProxy::~MemcacheProxy() {
}

string MemcacheProxy::get(const string key, const int32_t flags) {
  std::vector<char> ret_value;
  client->get(key, ret_value);
  string string_ret_value(ret_value.begin(), ret_value.end());
  return string_ret_value;
};

void MemcacheProxy::set(const string key, const string value, const int32_t flags, const int32_t expireInterval) {
  client->set(key, value.c_str(), value.length(), expireInterval, flags);
};
