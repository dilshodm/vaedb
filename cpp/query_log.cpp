#include "query_log.h"

QueryLogEntry::QueryLogEntry(QueryLog & log)
  : _log(log), _subdomain(""), _method_name("") { 
  boost::mutex::scoped_lock(_log._mutex);
  _query_id = _log._query_id++;
}

QueryLogEntry::~QueryLogEntry() {
  if(!is_logging())
    return;

  boost::posix_time::time_duration delta 
    = boost::posix_time::microsec_clock::universal_time() - _start_time;

  _sslog << "# end " << _method_name << " query " << _query_id << " after " 
         << delta.total_microseconds() << " microseconds";

  if(_subdomain.size() > 0)
    _sslog << " for " << _subdomain;

  _sslog << "." << std::endl << std::endl;

  flush();
}

QueryLogEntry & QueryLogEntry::set_subdomain(std::string const & subdomain) {
  _subdomain = subdomain;
}

QueryLogEntry & QueryLogEntry::method_call(std::string const & method_name) {
  if(is_logging()) {
    _method_name = method_name;
    _sslog << "# begin query " << _query_id << std::endl;
    _sslog << "method_call(" << method_name << "):" << std::endl;
    _start_time = boost::posix_time::microsec_clock::universal_time();
  }

  return *this;
}

bool QueryLogEntry::is_logging() {
  return _log._p_out != 0;
}

void QueryLogEntry::clear() { _sslog.str(std::string()); }

QueryLogEntry & QueryLogEntry::operator<< (QueryEntryManipulator manip) {
  manip(*this);
  return *this;
}

void QueryLogEntry::flush() {
  if(!is_logging())
    return;

  std::string log_line(_sslog.str());

  clear();
  {
    boost::mutex::scoped_lock(_log._mutex);
    (*_log._p_out) << log_line;
    _log._p_out->flush();
  }

}

void end(QueryLogEntry & entry) {
  entry.flush();
}

QueryLog::QueryLog(std::ostream * p_out)
  : _p_out(p_out), _query_id(0) {}

 
