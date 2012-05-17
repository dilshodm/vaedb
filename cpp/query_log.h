#ifndef _VAE_THRIFT_QUERY__LOG_H_
#define _VAE_THRIFT_QUERY__LOG_H_

#include <map>
#include <sstream>

template <typename T> 
struct TypeHelper {
  static std::string Name();
  static std::string ValueRep(const T & val);
};

template <>
template <typename K, typename V>
struct TypeHelper<std::map<K,V> > {
  static std::string Name() {
    return std::string("map<"+TypeHelper<K>::Name()+","+TypeHelper<V>::Name()+">");
  } 

  static std::string ValueRep(const std::map<K,V> & val) {
    std::stringstream ss;

    ss << "{";

    typename std::map<K,V>::const_iterator iter = val.begin();
    for(; iter != val.end(); ++iter)
        ss << TypeHelper<K>::ValueRep(iter->first) << ":" << TypeHelper<V>::ValueRep(iter->second) << ",";

    ss << "}";
    return ss.str();
  }
};

#define DEFINE_SIMPLE_LOGNAME(T, name)         \
template <>                                    \
struct TypeHelper< T > {                       \
  static std::string Name() {                  \
    return name ;                              \
  }                                            \
  static std::string ValueRep(const T & val) { \
    std::stringstream ss;                      \
    ss << val;                                 \
    return ss.str();                           \
  }                                            \
}                                              \


DEFINE_SIMPLE_LOGNAME(bool, "bool");
DEFINE_SIMPLE_LOGNAME(int32_t, "i32");
DEFINE_SIMPLE_LOGNAME(std::string, "string");

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
