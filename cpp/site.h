#ifndef _VAE_THRIFT_SITE_H_
#define _VAE_THRIFT_SITE_H_

#include <boost/thread/mutex.hpp>

#include <ctime>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "../gen-cpp/VaeDb.h"

typedef vector<class Context *> ContextList;
typedef map<int,xmlNodePtr> NodeIdMap;
typedef vector<xmlNodePtr> NodeList;
typedef map<string,xmlNodePtr> PermalinkMap;
typedef map<int,VaeDbStructure *> StructureMap;

class Site {
  
  string subdomain;
  string secretKey;
  StructureMap structures;
  
 public:
  ContextList associationsToInitialize;
  xmlDocPtr doc;
  boost::mutex mutex;
  NodeIdMap nodes;
  NodeList nodesToClean;
  NodeList nodesToClean2;
  PermalinkMap permalinks;
  xmlNodePtr rootDesignNode;
  xmlNodePtr rootNode;
  bool stagingMode;
  
  Site(string su, string sk, bool stagingMode);
  ~Site();
  string getSubdomain();
  void reset();
  VaeDbStructure *structureFromStructureId(int structureId);
  void validateSecretKey(string testSecretKey);

 private:
  void freeContexts(xmlNodePtr node);
  void loadXmlDoc();
  void validateSecretKeyAgainstConfig(string testSecretKey);

};

#endif
