#ifndef _VAE_THRIFT_SITE_H_
#define _VAE_THRIFT_SITE_H_

#include <boost/thread/mutex.hpp>

#include <ctime>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>


#include "memory_mgmt.h"
#include "context.h"
#include "query.h"
#include "lru_cache.h"
#include "../gen-cpp/VaeDb.h"

extern int testMode;

typedef vector<class Context *> ContextList;
typedef map<int,xmlNodePtr> NodeIdMap;
typedef vector<xmlNodePtr> NodeList;
typedef map<string,xmlNodePtr> PermalinkMap;
typedef map<int,VaeDbStructure *> StructureMap;

struct LRUKey {
  Context * p;
  string s;
};

inline
bool operator<(LRUKey const & a, LRUKey const & b) {
  return (a.p < b.p) || (a.p == b.p && a.s < b.s);
}

typedef LRUCache<LRUKey, boost::shared_ptr<Query> > QueryCache;

class Site : public gc {
  
  string subdomain;
  string secretKey;
  StructureMap structures;
 
 public:
  ContextList associationsToInitialize;
  QueryCache query_cache;
  xmlDocPtr doc;
  boost::mutex mutex;
  NodeIdMap nodes;
  NodeList nodesToClean;
  NodeList nodesToClean2;
  PermalinkMap permalinks;
  xmlNodePtr rootDesignNode;
  xmlNodePtr rootNode;
  bool stagingMode;
  
  Site(string su, bool stagingMode, string const & rawxml);
  ~Site();
  string getSubdomain();
  void reset();
  VaeDbStructure *structureFromStructureId(int structureId);
  void validateSecretKey(string testSecretKey);
  boost::shared_ptr<Query> fetch_query(LRUKey const & key);

 private:
  void freeContexts(xmlNodePtr node);
  void loadXmlDoc(const string & rawxml);

};

#endif
