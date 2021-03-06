#ifndef _VAE_THRIFT_BUS_H_
#define _VAE_THRIFT_BUS_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include "server.h"

class Bus {
  std::string _bindaddress;
  boost::shared_ptr<Server> _server;
  boost::shared_ptr<boost::thread> t;

  void reload(std::string subdomain);

public:
  Bus(boost::shared_ptr<Server> server, std::string bindaddress);
  void run();
};

#endif
