#include "mysql_proxy.h"

class Reaper {

  class VaeDbHandler *vaeDbHandler;
  MysqlProxy &mysqlProxy;

 public:
   Reaper(class VaeDbHandler *h, MysqlProxy &p);
   void Run();

};
