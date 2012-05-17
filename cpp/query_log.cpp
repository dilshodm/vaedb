#include "query_log.h"
boost::mutex QueryLog::_mutex;

QueryLogEntry::QueryLogEntry(QueryLog & log)
  : _log(log) { }

QueryLogEntry::QueryLogEntry(QueryLogEntry const & entry) 
  : _log(entry._log) {
  _sslog << entry._sslog.rdbuf();
}

QueryLogEntry & QueryLogEntry::method_call(std::string const & method_name) {
  if(is_logging())
    _sslog << "method_call(" << method_name << "):" << std::endl;

  return *this;
}

bool QueryLogEntry::is_logging() {
  return _log._p_out != 0;
}

void QueryLogEntry::clear() { _sslog.clear(); }

QueryLogEntry & QueryLogEntry::operator<< (QueryEntryManipulator manip) {
  manip(*this);
  return *this;
}

QueryLogEntry QueryLog::entry() {
  return QueryLogEntry(*this);
}

void end(QueryLogEntry & entry) {
  QueryLog & ql(entry._log);
  if(!entry.is_logging())
    return;

  boost::mutex::scoped_lock(ql._mutex);
  (*ql._p_out) << entry._sslog.str();
  ql._p_out->flush();

  entry.clear();
} 
