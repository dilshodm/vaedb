#include "mysql_proxy.h"

class Reaper {

  class Server *server;
  MysqlProxy &mysqlProxy;

 public:
   Reaper(class Server *h, MysqlProxy &p);
   void Run();

};
