#include <string>

#include "logger.h"
#include "mysql_proxy.h"

using std::string;

MysqlProxy::MysqlProxy(std::string host, std::string username, std::string password, std::string database) {
  L(info) << " Connecting to Mysql Server: " << host;
}

MysqlProxy::~MysqlProxy() {
}
