#ifndef _VAE_THRIFT_QUERY__LOG_H_
#define _VAE_THRIFT_QUERY__LOG_H_

#include <map>

template <typename T> 
struct TypeHelper {
  static std::string Name();
};

template <>
template <typename K, typename V>
struct TypeHelper<std::map<K,V> > {
  static std::string Name() {
    return std::string("map<"+TypeHelper<K>::Name()+","+TypeHelper<V>::Name()+">");
  } 
};

#define NAME(name) #name
#define DEFINE_SIMPLE_LOGNAME(type, name) \
template <>                         \
struct TypeHelper< type > {         \
  static std::string Name() {       \
    return NAME(name);              \
  }                                 \
}


DEFINE_SIMPLE_LOGNAME(bool, "bool");
DEFINE_SIMPLE_LOGNAME(int32_t, "i32");
DEFINE_SIMPLE_LOGNAME(std::string, "string");

struct QueryLog {
  QueryLog(ostream * p_out) : _p_out(p_out) {}

  QueryLog & method_call(std::string const & methodName) {
    if(_p_out)
      (*_p_out) << "method_call(methodName):" << std::endl;

    return *this;
  }

  template <typename T>
  QueryLog & operator<< (const T & value) {
    if(_p_out)
      (*_p_out) << "  " << TypeHelper<T>::Name() << ":" << value << std::endl;
    return *this;
  }

  private:
  ostream * _p_out;
};

template <typename K, typename V>
ostream& operator<< (ostream& out, std::map<K,V> const & sermap)
{
  typename std::map<K,V>::const_iterator iter = sermap.begin();
  out << "{";
  for(; iter != sermap.end(); ++iter) {
    out << iter->first << ":" << iter->second << ",";
  }
  out << "}";
  return out;
}

#endif // #ifndef _VAE_THRIFT_QUERY__LOG_H_
