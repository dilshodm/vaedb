#include "query_log.h"
boost::mutex QueryLog::_mutex;

void end(QueryLogEntry & entry) {
  QueryLog & ql(entry._log);
  boost::mutex::scoped_lock(ql._mutex);

  if(ql._p_out) {
    (*ql._p_out) << entry._sslog.str();
    ql._p_out->flush();
  }

  entry.clear();
} 
