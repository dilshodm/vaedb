#ifndef _VAE_THRIFT_BUS_H_
#define _VAE_THRIFT_BUS_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include "vae_db_handler.h"

class Bus {
  std::string _bindaddress;
  boost::shared_ptr<VaeDbHandler> _handler;

  void run();
  void reload(std::string subdomain);

public:
  Bus(boost::shared_ptr<VaeDbHandler> handler, std::string bindaddress);
};

#endif
