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

#include "../gen-cpp/VaeDb.h"
#include "logger.h"
#include "reaper.h"
#include "session.h"
#include "vae_db_handler.h"

Reaper::Reaper(VaeDbHandler *h, MysqlProxy &p) : vaeDbHandler(h), mysqlProxy(p) {
  thread *t = new thread(boost::bind(&Reaper::Run, this));
}

void Reaper::Run() {
  while (1) {
    sleep(SESSION_REAP_TIME / 2);
    time_t now = time(NULL);
    {
      boost::unique_lock<boost::mutex> lock(vaeDbHandler->sessionsMutex);
      SessionMap &sessions = vaeDbHandler->getSessions();

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
