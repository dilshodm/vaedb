#include <string>
#include <unistd.h>

#include "logger.h"
#include "mysql_proxy.h"

using namespace std;

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
  if (con) delete con;
}

void MysqlProxy::garbageCollect() {
  sql::PreparedStatement *stmt;
  try {
    stmt = con->prepareStatement("DELETE FROM `session_data` WHERE `expires`<UNIX_TIMESTAMP()");
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Garbage Collecting the Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return;
  }
  if (stmt) delete stmt; 
}

string MysqlProxy::longTermCacheGet(string subdomain, string key, int32_t renewExpiry) {
  string out = "";
  sql::PreparedStatement *stmt, *stmt2;
  sql::ResultSet *res;
  try {
    stmt = con->prepareStatement("SELECT `v`,DATEDIFF(`expire_at`,NOW()) AS remaining_days FROM `kvstore` WHERE `k`=? AND `subdomain`=?");
    stmt2 = con->prepareStatement("UPDATE kvstore SET `expire_at`=DATE_ADD(NOW(),INTERVAL ? DAY) WHERE `k`=? AND `subdomain`=?");
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    res = stmt->executeQuery();
    while (res->next()) {
      out = res->getString("v");
      if (renewExpiry > 0 && res->getInt("remaining_days") < 10) {
        stmt2->setInt(1, renewExpiry);
        stmt2->setString(2, key);
        stmt2->setString(3, subdomain);
        stmt2->execute();
      }
    }
    delete res; 
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Getting From the Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return "";
  }
  if (stmt) delete stmt; 
  if (stmt2) delete stmt2; 
  return out;
}

void MysqlProxy::longTermCacheSet(string subdomain, string key, string value, int32_t expireInterval, int32_t isFilename) {
  sql::PreparedStatement *stmt, *stmt2;
  try {
    stmt = con->prepareStatement("DELETE FROM `kvstore` WHERE `subdomain`=? AND `k`=?");
    stmt2 = con->prepareStatement("INSERT INTO `kvstore` (`k`,`subdomain`,`v`,`expire_at`,`is_filename`) VALUES(?,?,?,DATE_ADD(NOW(), INTERVAL ? DAY),?)");
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
    return;
  }
  if (stmt) delete stmt; 
  if (stmt2) delete stmt2; 
}

void MysqlProxy::longTermCacheEmpty(string subdomain) {
  sql::PreparedStatement *stmt;
  try {
    stmt = con->prepareStatement("DELETE FROM `kvstore` WHERE `subdomain`=?");
    stmt->setString(1, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Emptying The Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return;
  }
  if (stmt) delete stmt; 
}

void MysqlProxy::longTermCacheDelete(string subdomain, string key) {
  sql::PreparedStatement *stmt;
  try {
    stmt = con->prepareStatement("DELETE FROM `kvstore` WHERE `k`=? AND `subdomain`=?");
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Deleting From The Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return;
  }
  if (stmt) delete stmt; 
}

void MysqlProxy::longTermCacheSweeperInfo(VaeDbDataForContext& _return, string subdomain) {
  sql::PreparedStatement *stmt, *stmt2;
  sql::ResultSet *res;
  try {
    stmt = con->prepareStatement("SELECT `k`,`v` FROM `kvstore` WHERE `is_filename`='1' AND `subdomain`=?");
    stmt2 = con->prepareStatement("DELETE FROM `kvstore` WHERE `expire_at`<NOW() AND `subdomain`=?");
    stmt2->setString(1, subdomain);
    stmt2->execute();
    stmt->setString(1, subdomain);
    res = stmt->executeQuery();
    while (res->next()) {
      _return.data[res->getString("k")] = res->getString("v");
    }
    delete res; 
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Getting From the Long Term Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return;
  }
  if (stmt) delete stmt; 
  if (stmt2) delete stmt2; 
}

string MysqlProxy::sessionCacheGet(string subdomain, string key) {
  string out = "";
  sql::PreparedStatement *stmt;
  sql::ResultSet *res;
  try {
    stmt = con->prepareStatement("SELECT `data` FROM `session_data` WHERE `id`=? AND `subdomain`=?");
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    res = stmt->executeQuery();
    while (res->next()) {
      out = res->getString("data");
    }
    delete res; 
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Getting From the Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return "";
  }
  if (stmt) delete stmt; 
  return out;
}

void MysqlProxy::sessionCacheSet(string subdomain, string key, string value) {
  sql::PreparedStatement *stmt, *stmt2;
  try {
    stmt = con->prepareStatement("DELETE FROM `session_data` WHERE `id`=? AND `subdomain`=?");
    stmt2 = con->prepareStatement("INSERT INTO `session_data` (`id`,`subdomain`,`data`,`expires`) VALUES(?,?,?,UNIX_TIMESTAMP()+?)");
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
    return;
  }
  if (stmt) delete stmt; 
  if (stmt2) delete stmt2; 
}

void MysqlProxy::sessionCacheDelete(string subdomain, string key) {
  sql::PreparedStatement *stmt;
  try {
    stmt = con->prepareStatement("DELETE FROM `session_data` WHERE `id`=? AND `subdomain`=?");
    stmt->setString(1, key);
    stmt->setString(2, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Deleting From The Session Cache: " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return;
  }
  if (stmt) delete stmt; 
}

int32_t MysqlProxy::lock(string subdomain) {
  int32_t lockTime = 1000000;
  int32_t oldLocksRemoved = 0;
  int32_t gotLock = 0;
  sql::PreparedStatement *stmt, *stmt2;
  stmt->setString(1, subdomain);
  for (int i = 0; i < 60*10001000/lockTime; i++) {
    try {
      stmt = con->prepareStatement("INSERT INTO `locks` (subdomain,created_at) VALUES(?,NOW())");
      stmt->execute();
      gotLock = 1;
      break;
    } catch(sql::SQLException &e) {
      if (!oldLocksRemoved) {
        try {
          stmt2 = con->prepareStatement("DELETE FROM `locks` WHERE created_at<DATE_SUB(NOW(), INTERVAL 1 MINUTE)");
          stmt2->execute();
        } catch(sql::SQLException &e) {
          L(error) << "MySQL Error Removing Old Locks For " << subdomain << ": " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
          return gotLock;
        }
        oldLocksRemoved = 1;
      }  
      usleep(lockTime);
    }
  }
  if (stmt) delete stmt; 
  if (stmt2) delete stmt2; 
  return gotLock;
}

int32_t MysqlProxy::unlock(string subdomain) {
  sql::PreparedStatement *stmt;
  try {
    stmt = con->prepareStatement("DELETE FROM `locks` WHERE `subdomain`=?");
    stmt->setString(1, subdomain);
    stmt->execute();
  } catch(sql::SQLException &e) {
    L(error) << "MySQL Error Releasing Lock For " << subdomain << ": " << e.what() << " (Error Code: " << e.getErrorCode() << ")";
    return 0;
  }
  if (stmt) delete stmt; 
  return 1;
}
