#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

#include "bus.h"
#include <iostream>
#include <zmq.hpp>
#include "logger.h"

using std::string;

Bus::Bus(shared_ptr<VaeDbHandler> handler, string bindaddress) 
    : _handler(handler), _bindaddress(bindaddress) { }

void Bus::run() {
  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_PULL);

  subscriber.bind(_bindaddress.c_str());

  while(true) {
    zmq::message_t update;
    subscriber.recv(&update);

    string subdomain(static_cast<char*>(update.data()), update.size());

    L(info) << "reloading " << subdomain;
    _handler->reloadSite(subdomain);
  }
}
