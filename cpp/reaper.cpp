// not to be feared

#define SESSION_REAP_TIME 50

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <time.h>

using namespace boost;
using namespace std;

#include "../gen-cpp/VerbDb.h"
#include "logger.h"
#include "reaper.h"
#include "session.h"
#include "verb_db_handler.h"

Reaper::Reaper(VerbDbHandler *h) : verbDbHandler(h) {
  thread *t = new thread(boost::bind(&Reaper::Run, this));
}

void Reaper::Run() {
  while (1) {
    sleep(SESSION_REAP_TIME);
    time_t now = time(NULL);
    SessionMap &sessions = verbDbHandler->getSessions();
    for (SessionMap::iterator it = sessions.begin(); it != sessions.end(); it++) {
      if ((now - it->second->getCreatedAt()) > SESSION_REAP_TIME) {
        sessions.erase(it);
      }
    }
  }
}
