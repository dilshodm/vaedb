#ifndef _VAE_THRIFT_MYSQL_PROXY_H_
#define _VAE_THRIFT_MYSQL_PROXY_H_

#include <string>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "../gen-cpp/VaeDb.h"

class MysqlProxy {
  sql::Driver *driver;
  sql::Connection *con;
 
public:
  MysqlProxy(std::string host, std::string username, std::string password, std::string database);
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
};

#endif
