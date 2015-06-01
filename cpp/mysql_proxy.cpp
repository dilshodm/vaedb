#include <string>

#include "logger.h"
#include "mysql_proxy.h"

using std::string;

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

MysqlProxy::MysqlProxy(string host, string username, string password, string database) {
  L(info) << " Connecting to Mysql Server: " << host;
  driver = get_driver_instance();
  try {
    con = driver->connect("tcp://" + host + ":3306", username, password);
  } catch(sql::SQLException &e) {
    L(error) << "Error Connecting To MySQL: " << e.what();
  }
  try {
    con->setSchema(database);
  } catch(sql::SQLException &e) {
    L(error) << "Error Selecting Database in MySQL: " << e.what();
  }
}

MysqlProxy::~MysqlProxy() {
  delete con;
}

string MysqlProxy::sessionCacheGet(string subdomain, string key) {
  string out = "";
  sql::PreparedStatement *stmt = con->prepareStatement("SELECT `data` FROM `session_data` WHERE `id`=? AND `subdomain`=?");
  sql::ResultSet *res;
  try {
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    res = stmt->executeQuery();
    while (res->next()) {
      out = res->getString("data");
    }
    delete res; 
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Getting From the Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
  delete stmt; 
  return out;
}

void MysqlProxy::sessionCacheSet(string subdomain, string key, string value) {
// TODO: Garbage collector
// TODO: HAndle expiration
  sql::PreparedStatement *stmt = con->prepareStatement("INSERT INTO `session_data` (`id`,`subdomain`,`data`,`expires`) VALUES(?,?,?,?)");
  try {
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    stmt->setString(3, value);
    stmt->setString(4, 0);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Saving to the Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
  delete stmt; 
}

void MysqlProxy::sessionCacheDelete(string subdomain, string key) {
  sql::PreparedStatement *stmt = con->prepareStatement("DELETE FROM `session_data` WHERE `id`=? AND `subdomain`=?");
  try {
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Deleting From The Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
  delete stmt; 
}

int32_t MysqlProxy::lock(string subdomain) {
  sql::PreparedStatement *stmt = con->prepareStatement("INSERT INTO `locks` (id) VALUES(?)");
  try {
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Fetching Lock For " << subdomain << ": " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
  delete stmt; 
  return 0;
}

int32_t MysqlProxy::unlock(string subdomain) {
  sql::PreparedStatement *stmt = con->prepareStatement("DELETE FROM `locks` WHERE `id`=?");
  try {
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Releasing Lock For " << subdomain << ": " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
  delete stmt; 
  return 0;
}
