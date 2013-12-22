#ifndef _VAE_THRIFT_BUS_H_
#define _VAE_THRIFT_BUS_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include "thrift/concurrency/Thread.h"
#include "vae_db_handler.h"

class Bus : public apache::thrift::concurrency::Runnable {
  std::string _bindaddress;
  boost::shared_ptr<VaeDbHandler> _handler;

  void run();

public:
  Bus(boost::shared_ptr<VaeDbHandler> handler, std::string bindaddress);
};

#endif
