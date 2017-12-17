#ifndef _VAE_THRIFT_MEMCACHE_PROXY_H_
#define _VAE_THRIFT_MEMCACHE_PROXY_H_

#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <libmemcached/memcached.hpp>

enum {
  MEMCACHE_POOL_FREE,
  MEMCACHE_POOL_INUSE
};

typedef std::map<memcache::Memcache *, short> MemcachePool;

class MemcacheProxy {
  std::string connectString;
  MemcachePool connPool;
  boost::mutex poolMutex;
  boost::condition_variable cond;
  int availableConnections;
 
public:
  MemcacheProxy(std::string connectString, int workers);
  ~MemcacheProxy();
  std::string get(const std::string key, const int32_t flags);
  void set(const std::string key, const std::string value, const int32_t flags, const int32_t expireInterval);
  void remove(const std::string key);

private:
  memcache::Memcache *createConnection();
  boost::shared_ptr<memcache::Memcache> getConnection();
  void freeConnection(memcache::Memcache *con);
};

#endif
