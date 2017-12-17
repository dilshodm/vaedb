#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;
using namespace boost;

#include "bus.h"
#include <iostream>
#include "zmq.hpp"
#include "logger.h"

using std::string;

Bus::Bus(boost::shared_ptr<VaeDbHandler> handler, string bindaddress) 
    : _handler(handler), _bindaddress(bindaddress) { }

void Bus::reload(string subdomain) {
  _handler->reloadSite(subdomain);
}

void Bus::run() {
  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_PULL);
  vector<boost::thread*> threads;
  
  boost::posix_time::seconds waittime(0);

  subscriber.bind(_bindaddress.c_str());
  while(true) {
    zmq::message_t update;
    subscriber.recv(&update);

    string subdomain(static_cast<char*>(update.data()), update.size());

    L(info) << "reloading " << subdomain;
    _handler->reloadSite(subdomain);

    vector<boost::thread*>::iterator it = threads.begin();
    while(it != threads.end()) {
        if((*it)->timed_join(waittime)) {
            delete *it;
            it = threads.erase(it);
        }
        else {
            ++it;
        }
    }

    threads.push_back(new boost::thread(&Bus::reload, this, subdomain));
  }
}
