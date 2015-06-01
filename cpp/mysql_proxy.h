#ifndef _VAE_THRIFT_MYSQL_PROXY_H_
#define _VAE_THRIFT_MYSQL_PROXY_H_

#include <string>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class MysqlProxy {
  sql::Driver *driver;
  sql::Connection *con;
 
public:
  MysqlProxy(std::string host, std::string username, std::string password, std::string database);
  ~MysqlProxy();
};

#endif
