// not to be feared

#define SESSION_REAP_TIME 62
// ^ needs to be set at least [PHP script timeout in seconds] + some small safety margin (~2)

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <iostream>
#include <time.h>

using namespace boost;
using namespace std;

#include "logger.h"
#include "reaper.h"
#include "server.h"
#include "session.h"

Reaper::Reaper(Server *h, MysqlProxy &p) : server(h), mysqlProxy(p) {
  std::thread *t = new std::thread(boost::bind(&Reaper::Run, this));
}

void Reaper::Run() {
  while (1) {
    sleep(SESSION_REAP_TIME / 2);
    time_t now = time(NULL);
    {
      boost::unique_lock<boost::mutex> lock(server->sessionsMutex);
      SessionMap &sessions = server->getSessions();

      SessionMap::iterator it = sessions.begin();
      while (it != sessions.end()) {
        if ((now - it->second->getCreatedAt()) > SESSION_REAP_TIME) {
          sessions.erase(it++);
          L(info) << "Erasing Session";
        } else { ++it; }
      }
    }

    mysqlProxy.garbageCollect();
  }
}
