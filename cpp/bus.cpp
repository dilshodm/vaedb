#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost;

#include "bus.h"
#include <iostream>
#include <sstream>
#include <zmq.hpp>

Bus::Bus(shared_ptr<VaeDbHandler> handler) : _handler(handler) { }

void Bus::run() {
  zmq::context_t context (1);
  zmq::socket_t subscriber (context, ZMQ_SUB);

  subscriber.bind("tcp://127.0.0.1:5558");
  subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

  while (true) {
    zmq::message_t update;
    std::string subdomain;

    subscriber.recv(&update);

    std::istringstream iss(static_cast<char*>(update.data()));
    iss >> subdomain;
    std::cout << "got zmq req: " << subdomain << std::endl;
    _handler->reloadSite(subdomain);
  }
}
