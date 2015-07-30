#include <functional>
#include <string>
#include <unistd.h>
#include <boost/smart_ptr.hpp>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "logger.h"
#include "mysql_proxy.h"

using namespace std;


MysqlProxy::MysqlProxy(string host, string username, string password, string database) 
  : host(host), username(username), password(password), database(database) {
  L(info) << " Connecting to MySQL Server: " << host;
  for (int i = 0; i < NUM_CONNECTIONS; i++) {
    sql::Connection *con = this->createConnection();
    if (con) {
      connPool[con] = POOL_FREE;
      this->availableConnections++;
    }
  }
}

MysqlProxy::~MysqlProxy() {
}

sql::Connection *MysqlProxy::createConnection() {
  sql::Connection *con;
  sql::ConnectOptionsMap opts;
  driver = get_driver_instance();
  try {
    opts["hostName"] = "tcp://" + host + ":3306";
    opts["userName"] = username;
    opts["password"] = password;
    opts["OPT_RECONNECT"] = true;
    con = driver->connect(opts);
  } catch(sql::SQLException &e) {
    L(error) << "Error Connecting To MySQL: " << e.what();
    return NULL;
  }
  try {
    con->setSchema(database);
  } catch(sql::SQLException &e) {
    L(error) << "Error Selecting Database in MySQL: " << e.what();
    return NULL;
  }
  return con;
}

boost::shared_ptr<sql::Connection> MysqlProxy::getConnection() {
  boost::unique_lock<boost::mutex> lock(poolMutex);
  while (this->availableConnections == 0) {
    this->cond.wait(lock);
  }
  for (Pool::iterator iter = connPool.begin(); iter != connPool.end(); iter++) {
    if (iter->second == POOL_FREE) {
      iter->second = POOL_INUSE;
      this->availableConnections--;
      return boost::shared_ptr<sql::Connection>(iter->first, std::bind(&MysqlProxy::freeConnection, this, std::placeholders::_1));
    }
  }
  throw "MySQL Thread Pool Error: Should never get here";
}

void MysqlProxy::freeConnection(sql::Connection *con) {
  boost::unique_lock<boost::mutex> lock(poolMutex);
  for (Pool::iterator iter = connPool.begin(); iter != connPool.end(); iter++) {
    if (iter->first == con) {
      iter->second = POOL_FREE;
      this->availableConnections++;
    }
  }
  cond.notify_one();
}

void MysqlProxy::garbageCollect() {
  boost::shared_ptr<sql::PreparedStatement> stmt;
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `session_data` WHERE `expires`<UNIX_TIMESTAMP()"));
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Garbage Collecting the Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return;
  }
}

string MysqlProxy::longTermCacheGet(string subdomain, string key, int32_t renewExpiry) {
  string out = "";
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  boost::shared_ptr<sql::PreparedStatement> stmt2;
  boost::shared_ptr<sql::ResultSet> res;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("SELECT `v`,DATEDIFF(`expire_at`,NOW()) AS remaining_days FROM `kvstore` WHERE `k`=? AND `subdomain`=?"));
    stmt2 = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("UPDATE kvstore SET `expire_at`=DATE_ADD(NOW(),INTERVAL ? DAY) WHERE `k`=? AND `subdomain`=?"));
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    res = boost::shared_ptr<sql::ResultSet>(stmt->executeQuery());
    while (res->next()) {
      out = res->getString("v");
      if (renewExpiry > 0 && res->getInt("remaining_days") < 10) {
        stmt2->setInt(1, renewExpiry);
        stmt2->setString(2, key);
        stmt2->setString(3, subdomain);
        stmt2->execute();
      }
    }
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Getting From the Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
  return out;
}

void MysqlProxy::longTermCacheSet(string subdomain, string key, string value, int32_t expireInterval, int32_t isFilename) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  boost::shared_ptr<sql::PreparedStatement> stmt2;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `kvstore` WHERE `subdomain`=? AND `k`=?"));
    stmt2 = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("INSERT INTO `kvstore` (`k`,`subdomain`,`v`,`expire_at`,`is_filename`) VALUES(?,?,?,DATE_ADD(NOW(), INTERVAL ? DAY),?)"));
    stmt->setString(1, subdomain);
    stmt->setString(2, key);
    stmt->execute();
    stmt2->setString(1, key);
    stmt2->setString(2, subdomain);
    stmt2->setString(3, value);
    stmt2->setInt(4, expireInterval);
    stmt2->setInt(5, isFilename);
    stmt2->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Saving to the Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
}

void MysqlProxy::longTermCacheEmpty(string subdomain) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `kvstore` WHERE `subdomain`=?"));
    stmt->setString(1, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Emptying The Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
}

void MysqlProxy::longTermCacheDelete(string subdomain, string key) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `kvstore` WHERE `k`=? AND `subdomain`=?"));
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Deleting From The Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
}

void MysqlProxy::longTermCacheSweeperInfo(VaeDbDataForContext& _return, string subdomain) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  boost::shared_ptr<sql::PreparedStatement> stmt2;
  boost::shared_ptr<sql::ResultSet> res;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("SELECT `k`,`v` FROM `kvstore` WHERE `is_filename`='1' AND `subdomain`=?"));
    stmt2 = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `kvstore` WHERE `expire_at`<NOW() AND `subdomain`=?"));
    stmt2->setString(1, subdomain);
    stmt2->execute();
    stmt->setString(1, subdomain);
    res = boost::shared_ptr<sql::ResultSet>(stmt->executeQuery());
    while (res->next()) {
      _return.data[res->getString("k")] = res->getString("v");
    }
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Getting From the Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
}

string MysqlProxy::sessionCacheGet(string subdomain, string key) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  boost::shared_ptr<sql::ResultSet> res;
  string out = "";
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("SELECT `data` FROM `session_data` WHERE `id`=? AND `subdomain`=?"));
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    res = boost::shared_ptr<sql::ResultSet>(stmt->executeQuery());
    while (res->next()) {
      out = res->getString("data");
    }
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Getting From the Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
  return out;
}

void MysqlProxy::sessionCacheSet(string subdomain, string key, string value) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  boost::shared_ptr<sql::PreparedStatement> stmt2;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `session_data` WHERE `id`=? AND `subdomain`=?"));
    stmt2 = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("INSERT INTO `session_data` (`id`,`subdomain`,`data`,`expires`) VALUES(?,?,?,UNIX_TIMESTAMP()+?)"));
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    stmt->execute();
    stmt2->setString(1, key);
    stmt2->setString(2, subdomain);
    stmt2->setString(3, value);
    stmt2->setInt(4, 86400*2);
    stmt2->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Saving to the Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
}

void MysqlProxy::sessionCacheDelete(string subdomain, string key) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `session_data` WHERE `id`=? AND `subdomain`=?"));
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Deleting From The Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
  }
}

int32_t MysqlProxy::lock(string subdomain) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  boost::shared_ptr<sql::PreparedStatement> stmt2;
  int32_t gotLock = 0;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("INSERT INTO `locks` (subdomain,created_at) VALUES(?,NOW())"));
    stmt->setString(1, subdomain);
    stmt->execute();
    gotLock = 1;
  } catch(sql::SQLException &e) {
    try {
      stmt2 = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `locks` WHERE created_at<DATE_SUB(NOW(), INTERVAL 1 MINUTE)"));
      stmt2->execute();
    } catch(sql::SQLException &e) {
      L(error) << "MySQL Error Removing Old Locks For " << subdomain << ": " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    }
  }
  return gotLock;
}

int32_t MysqlProxy::unlock(string subdomain) {
  boost::shared_ptr<sql::Connection> con = this->getConnection();
  boost::shared_ptr<sql::PreparedStatement> stmt;
  try {
    stmt = boost::shared_ptr<sql::PreparedStatement>(con->prepareStatement("DELETE FROM `locks` WHERE `subdomain`=?"));
    stmt->setString(1, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Releasing Lock For " << subdomain << ": " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return 0;
  }
  return 1;
}
