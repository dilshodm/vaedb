// not to be feared

#define SESSION_REAP_TIME 300

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <time.h>

using namespace boost;
using namespace std;

#include "../gen-cpp/VaeDb.h"
#include "logger.h"
#include "reaper.h"
#include "session.h"
#include "vae_db_handler.h"

Reaper::Reaper(VaeDbHandler *h) : vaeDbHandler(h) {
  thread *t = new thread(boost::bind(&Reaper::Run, this));
}

void Reaper::Run() {
  while (1) {
    sleep(SESSION_REAP_TIME);
    time_t now = time(NULL);
    SessionMap &sessions = vaeDbHandler->getSessions();

    SessionMap::iterator it = sessions.begin();
    while (it != sessions.end()) {
      if ((now - it->second->getCreatedAt()) > SESSION_REAP_TIME) {
        sessions.erase(it++);
      } else { ++it; }
    }
  }
}
