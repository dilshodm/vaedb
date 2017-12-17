#ifndef _VAE_THRIFT_MYSQL_PROXY_H_
#define _VAE_THRIFT_MYSQL_PROXY_H_

#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

enum {
  POOL_FREE,
  POOL_INUSE
};

typedef std::map<sql::Connection*, short> Pool;

class MysqlProxy {
  sql::Driver *driver;
  std::string username;
  std::string password;
  std::string database;
  std::string host;
  Pool connPool;
  boost::mutex poolMutex;
  boost::condition_variable cond;
  int availableConnections;

public:
  MysqlProxy(std::string host, std::string username, std::string password, std::string database, int workers);
  ~MysqlProxy();
  void garbageCollect();
  std::string sessionCacheGet(std::string subdomain, std::string key);
  void sessionCacheSet(std::string subdomain, std::string key, std::string value);
  void sessionCacheDelete(std::string subdomain, std::string key);
  std::string longTermCacheGet(std::string subdomain, std::string key, int32_t renewExpiry);
  void longTermCacheSet(std::string subdomain, std::string key, std::string value, int32_t expireInterval, int32_t isFilename);
  void longTermCacheDelete(std::string subdomain, std::string key);
  void longTermCacheEmpty(std::string subdomain);
  void longTermCacheSweeperInfo(VaeDbDataForContext& _return, std::string subdomain);
  int32_t lock(std::string subdomain);
  int32_t unlock(std::string subdomain);

private:
  sql::Connection *createConnection();
  boost::shared_ptr<sql::Connection> getConnection();
  void freeConnection(sql::Connection *con);
};

#endif
