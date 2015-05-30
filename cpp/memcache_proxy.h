#ifndef _VAE_THRIFT_MEMCACHE_PROXY_H_
#define _VAE_THRIFT_MEMCACHE_PROXY_H_

#include <string>
#include <libmemcached/memcached.hpp>

class MemcacheProxy {
  memcache::Memcache *client;
 
public:
  MemcacheProxy(std::string connectString);
  ~MemcacheProxy();
  std::string get(std::string key);
  void set(std::string key, std::string value);
};

#endif
