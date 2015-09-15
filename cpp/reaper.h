#include "memory_mgmt.h"
#include "mysql_proxy.h"

class Reaper : public gc {

  class VaeDbHandler *vaeDbHandler;
  MysqlProxy &mysqlProxy;

 public:
   Reaper(class VaeDbHandler *h, MysqlProxy &p);
   void Run();

};
