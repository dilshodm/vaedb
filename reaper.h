#include "mysql_proxy.h"

class Reaper {

  class Server *server;
  MysqlProxy &mysqlProxy;
  boost::shared_ptr<boost::thread> t;

 public:
   Reaper(class Server *h, MysqlProxy &p);
   void Run();

};
