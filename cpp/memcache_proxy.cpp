#include <string>
#include <pcrecpp.h>

#include "logger.h"
#include "memcache_proxy.h"

using std::string;

MemcacheProxy::MemcacheProxy(std::string c) {
  pcrecpp::RE(",").Replace(" --SERVER=", &c);
  connectString = "--TCP-KEEPALIVE --SERVER=" + c;
  L(info) << " Connecting to Memcache via: " << connectString;
  for (int i = 0; i < MEMCACHE_NUM_CONNECTIONS; i++) {
    memcache::Memcache *con = this->createConnection();
    if (con) {
      connPool[con] = MEMCACHE_POOL_FREE;
      this->availableConnections++;
    }
  }
}

MemcacheProxy::~MemcacheProxy() {
}

memcache::Memcache *MemcacheProxy::createConnection() {
  return new memcache::Memcache(connectString);
}

boost::shared_ptr<memcache::Memcache> MemcacheProxy::getConnection() {
  boost::unique_lock<boost::mutex> lock(poolMutex);
  while (this->availableConnections == 0) {
    this->cond.wait(lock);
  }
  for (MemcachePool::iterator iter = connPool.begin(); iter != connPool.end(); iter++) {
    if (iter->second == MEMCACHE_POOL_FREE) {
      iter->second = MEMCACHE_POOL_INUSE;
      this->availableConnections--;
      return boost::shared_ptr<memcache::Memcache>(iter->first, std::bind(&MemcacheProxy::freeConnection, this, std::placeholders::_1));
    }
  }
  throw "Memcache Thread Pool Error: Should never get here";
}

void MemcacheProxy::freeConnection(memcache::Memcache *con) {
  boost::unique_lock<boost::mutex> lock(poolMutex);
  for (MemcachePool::iterator iter = connPool.begin(); iter != connPool.end(); iter++) {
    if (iter->first == con) {
      iter->second = MEMCACHE_POOL_FREE;
      this->availableConnections++;
    }
  }
  cond.notify_one();
}

string MemcacheProxy::get(const string key, const int32_t flags) {
  boost::shared_ptr<memcache::Memcache> client = this->getConnection();
  std::vector<char> ret_value;
  client->get(key, ret_value);
  string string_ret_value(ret_value.begin(), ret_value.end());
  return string_ret_value;
};

void MemcacheProxy::set(const string key, const string value, const int32_t flags, const int32_t expireInterval) {
  boost::shared_ptr<memcache::Memcache> client = this->getConnection();
  const std::vector<char> valueVector(value.begin(), value.end());
  client->set(key, valueVector, expireInterval, flags);
};

void MemcacheProxy::remove(const string key) {
  boost::shared_ptr<memcache::Memcache> client = this->getConnection();
  client->remove(key);
};
