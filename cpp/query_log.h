#ifndef _VAE_THRIFT_QUERY__LOG_H_
#define _VAE_THRIFT_QUERY__LOG_H_

#include <map>
#include <sstream>
#include <boost/thread/mutex.hpp>

inline
void str_replace(std::string &str,
                 std::string const & search,
                 std::string const & replacement) {

  std::string::size_type loc = 0;
  while((loc = str.find(search, loc)) != std::string::npos)
    str.replace(loc++, search.size(), replacement);
}

template <typename T>
struct TypeHelper {
  inline static std::string Name();
  inline static std::string ValueRep(T const & val);
};

template <>
struct TypeHelper<std::string> {
  inline static std::string Name() {
    return "string";
  }
  inline static std::string ValueRep(std::string const & val) {
    std::stringstream ss;
    std::string escaped(val);
    str_replace(escaped, "\\", "\\\\");
    str_replace(escaped, "\"", "\\\"");
    ss << '"' << escaped << '"';
    return ss.str();
  }
};

template <>
template <typename K, typename V>
struct TypeHelper<std::map<K,V> > {
  inline static std::string Name() {
    return std::string("map<"+TypeHelper<K>::Name()+","+TypeHelper<V>::Name()+">");
  }

  inline static std::string ValueRep(std::map<K,V> const & val) {
    std::stringstream ss;
    ss << "{";

    typename std::map<K,V>::const_iterator iter = val.begin();
    for(; iter != val.end(); ++iter)
        ss << TypeHelper<K>::ValueRep(iter->first) << ":" << TypeHelper<V>::ValueRep(iter->second) << ",";

    ss << "}";
    return ss.str();
  }
};

#define DEFINE_SIMPLE_LOGNAME(T, name)                \
template <>                                           \
struct TypeHelper< T > {                              \
  inline static std::string Name() {                  \
    return name ;                                     \
  }                                                   \
  inline static std::string ValueRep(T const & val) { \
    std::stringstream ss;                             \
    ss << val;                                        \
    return ss.str();                                  \
  }                                                   \
}                                                     \


DEFINE_SIMPLE_LOGNAME(bool, "bool");
DEFINE_SIMPLE_LOGNAME(int32_t, "i32");

template <typename T>
std::string typename_of(T const & value) {
  return TypeHelper<T>::Name();
}

template <typename T>
std::string valuerep_of(T const & value) {
  return TypeHelper<T>::ValueRep(value);
}

struct QueryLog;
struct QueryLogEntry;

typedef void (&QueryEntryManipulator)(QueryLogEntry&);

struct QueryLogEntry {
  friend void end (QueryLogEntry &);

  QueryLogEntry(QueryLog & log) : _log(log) { }
  QueryLogEntry(QueryLogEntry const & entry) : _log(entry._log) { 
    _sslog << entry._sslog.rdbuf();
  }

  QueryLogEntry & method_call(std::string const & method_name) {
    _sslog << "method_call(" << method_name << "):" << std::endl;
    return *this;
  }

  template <typename T>
  QueryLogEntry & operator<< (T const & value) {
    _sslog << "  " 
      << typename_of(value) << ":"
      << valuerep_of(value)
      << std::endl;

    return *this;
  }

  QueryLogEntry & operator<< (QueryEntryManipulator manipulator) {
    manipulator(*this);
    return *this;
  }

  void clear() { _sslog.clear(); }

  private:
  QueryLog & _log;
  std::stringstream _sslog;
};

void end(QueryLogEntry & entry);

struct QueryLog {
  friend void end (QueryLogEntry &);

  QueryLog(std::ostream * p_out) : _p_out(p_out) {}

  QueryLogEntry entry() {
    return QueryLogEntry(*this);
  }

  private:
  std::ostream * _p_out;
  static boost::mutex _mutex;
};


#endif // #ifndef _VAE_THRIFT_QUERY__LOG_H_
