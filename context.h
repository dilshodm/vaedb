#ifndef _VAE_THRIFT_CONTEXT_H_
#define _VAE_THRIFT_CONTEXT_H_

#include <boost/thread/mutex.hpp>
#include <libxml/tree.h>

#include "json.hpp"

using namespace nlohmann;

typedef map<string,int> CachedCountMap;
typedef map<string,string> DataMap;
typedef vector<xmlNodePtr> NodeList;

struct Structure {
  int32_t id;
  string name;
  string type;
  string permalink;
};

class Context {

  CachedCountMap cachedCounts;
  DataMap cachedQueries;
  DataMap data;
  bool dataPopulated;
  int32_t id;
  boost::mutex mutex;
  char *permalink;
  NodeList pointerNodes;
  string singleData;
  class Site *site;
  Structure *structure;
  string type;

 public:
  xmlNodePtr node;
  bool killMe;

 private:
  string getSingleData();
  bool populateAttrs();
  void populateData();

 public:
  Context(class Site *s, const xmlNodePtr n);
  int getCount(const string &query);
  string getData(const string &query);
  int32_t getId();
  const char *getNodeName();
  int32_t getStructureId();
  void initializeAssociation();
  json toJson();
  DataMap getData();
  Structure getStructure();
};

#endif
