#ifndef _VAE_THRIFT_QUERY__LOG_H_
#define _VAE_THRIFT_QUERY__LOG_H_

#include <map>
#include <sstream>

inline
void str_replace(std::string &str,
                 std::string const & search,
                 std::string const & replacement) {

  string::size_type loc = 0;
  while((loc = str.find(search, loc)) != string::npos)
    str.replace(loc++, search.size(), replacement);
}

template <typename T>
struct TypeHelper {
  inline static std::string Name();
  inline static std::string ValueRep(const T & val);
};

template <>
struct TypeHelper<std::string> {
  inline static std::string Name() {
    return "string";
  }
  inline static std::string ValueRep(const std::string & val) {
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

  inline static std::string ValueRep(const std::map<K,V> & val) {
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
  inline static std::string ValueRep(const T & val) { \
    std::stringstream ss;                             \
    ss << val;                                        \
    return ss.str();                                  \
  }                                                   \
}                                                     \


DEFINE_SIMPLE_LOGNAME(bool, "bool");
DEFINE_SIMPLE_LOGNAME(int32_t, "i32");

struct QueryLog {
  QueryLog(ostream * p_out) : _p_out(p_out) {}

  QueryLog & method_call(std::string const & methodName) {
    if(_p_out)
      (*_p_out) << "method_call(" << methodName << "):" << std::endl;

    return *this;
  }

  template <typename T>
  QueryLog & operator<< (const T & value) {
    if(_p_out)
      (*_p_out) << "  " << TypeHelper<T>::Name() << ":"
                        << TypeHelper<T>::ValueRep(value) << std::endl;
    return *this;
  }

  private:
  ostream * _p_out;
};

#endif // #ifndef _VAE_THRIFT_QUERY__LOG_H_
