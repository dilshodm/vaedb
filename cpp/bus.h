#ifndef _VAE_THRIFT_BUS_H_
#define _VAE_THRIFT_BUS_H_

#include "thrift/concurrency/Thread.h"
#include "vae_db_handler.h"

class Bus : public apache::thrift::concurrency::Runnable {
  shared_ptr<VaeDbHandler> _handler;
  void run();
public:
  Bus(shared_ptr<VaeDbHandler> handler);
};

#endif
