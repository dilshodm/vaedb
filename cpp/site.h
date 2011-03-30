#include <boost/thread/mutex.hpp>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

typedef vector<class Context *> ContextList;
typedef map<int,xmlNodePtr> NodeIdMap;
typedef vector<xmlNodePtr> NodeList;
typedef map<string,xmlNodePtr> PermalinkMap;
typedef map<int,VerbDbStructure *> StructureMap;

class Site {
  
  string filename;
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
  
  Site(string su, string sk, bool testMode, bool stagingMode);
  ~Site();
  string getSubdomain();
  void reset();
  VerbDbStructure *structureFromStructureId(int structureId);
  void validateSecretKey(string testSecretKey);

 private:
  void freeContexts(xmlNodePtr node);
  void loadXmlDoc();
  void validateSecretKeyAgainstConfig(string testSecretKey);

};
