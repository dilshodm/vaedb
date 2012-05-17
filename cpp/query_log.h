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

struct QueryLog {
  friend void end (QueryLogEntry &);
  friend struct QueryLogEntry;

  QueryLog(std::ostream * p_out) : _p_out(p_out) {}
  QueryLogEntry entry();

  private:
  std::ostream * _p_out;
  static boost::mutex _mutex;
};

struct QueryLogEntry {
  friend void end (QueryLogEntry &);

  QueryLogEntry(QueryLog & log);
  QueryLogEntry(QueryLogEntry const & entry);

  QueryLogEntry & method_call(std::string const & method_name);
  QueryLogEntry & operator<< (QueryEntryManipulator manipulator);

  template <typename T>
  QueryLogEntry & operator<< (T const & value) {
    if(_log._p_out)
      _sslog << "  " 
        << typename_of(value) << ":"
        << valuerep_of(value)
        << std::endl;

    return *this;
  }


  private:
  void clear();
  bool is_logging();

  QueryLog & _log;
  std::stringstream _sslog;
};

void end(QueryLogEntry & entry);

#endif // #ifndef _VAE_THRIFT_QUERY__LOG_H_
