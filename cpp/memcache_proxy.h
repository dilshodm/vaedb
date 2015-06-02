#ifndef _VAE_THRIFT_MEMCACHE_PROXY_H_
#define _VAE_THRIFT_MEMCACHE_PROXY_H_

#include <boost/thread/mutex.hpp>
#include <string>
#include <libmemcached/memcached.hpp>

class MemcacheProxy {
  memcache::Memcache *client;
  boost::mutex clientMutex;
 
public:
  MemcacheProxy(std::string connectString);
  ~MemcacheProxy();
  std::string get(const std::string key, const int32_t flags);
  void set(const std::string key, const std::string value, const int32_t flags, const int32_t expireInterval);
  void remove(const std::string key);
};

#endif
