#include <string>
#include <pcrecpp.h>

#include "memcache_proxy.h"

using std::string;

MemcacheProxy::MemcacheProxy(std::string connectString) {
  pcrecpp::RE(",").Replace(" ", &connectString);
  client = new memcache::Memcache(connectString);
}

MemcacheProxy::~MemcacheProxy() {
}

string MemcacheProxy::get(string key) {
  std::vector<char> ret_value;
  client->get(key, ret_value);
  string string_ret_value(ret_value.begin(), ret_value.end());
  return string_ret_value;
};

void MemcacheProxy::set(string key, string value) {
  client->set(key, value.c_str(), value.length(), 0, 0);
};
