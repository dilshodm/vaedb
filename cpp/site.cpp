#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/smart_ptr.hpp>

using namespace boost;
using namespace std;

#include "../gen-cpp/VaeDb.h"
#include "context.h"
#include "site.h"
#include "logger.h"
#include "response.h"
#include "query.h"
#include "s3.h"

Site::Site(string su, bool stagingMode_, string const & rawxml) : stagingMode(stagingMode_), query_cache(384) {
  subdomain = su;
  if (!testMode) {
    secretKey = read_s3(subdomain+"-secret");
  }
  loadXmlDoc(rawxml);
  L(info) << "[" << subdomain << "] opened";
}
  
Site::~Site() {
  L(info) << "[" << subdomain << "] closed";
  for (NodeList::iterator it = nodesToClean.begin(); it != nodesToClean.end(); it++) {
    (*it)->last = (*it)->children = NULL;
    (*it)->properties = NULL;
    (*it)->_private = NULL;
  }
  for (NodeList::iterator it = nodesToClean2.begin(); it != nodesToClean2.end(); it++) {
    if ((*it)->children) (*it)->children->next = NULL;
    (*it)->last = (*it)->children;
    (*it)->properties = NULL;
    (*it)->_private = NULL;
  }
  for (StructureMap::iterator it = structures.begin(); it != structures.end(); it++) {
    delete it->second;
    it->second = NULL;
  }
  nodesToClean.clear();
  nodesToClean2.clear();
  freeContexts(rootNode);
  xmlFreeDoc(doc);
  nodes.clear();
  structures.clear();
}

void Site::freeContexts(xmlNodePtr node) {
  for (xmlNode *child = node->children; child; child = child->next) {
    if (child->type == XML_ELEMENT_NODE) {    
      if (child->_private != NULL) {
        delete (Context *)child->_private;
        child->_private = NULL;
      }
      freeContexts(child);
    }
  }
}

string Site::getSubdomain() {
  return subdomain;
}

void Site::loadXmlDoc(string const & rawxml) {
  if (testMode) {
    doc = xmlParseFile(subdomain.c_str());
    subdomain = "test";
  } else {
    doc = xmlReadMemory(rawxml.c_str(), rawxml.size(), subdomain.c_str(), NULL, 0);
  }
  if (doc == NULL) {
    L(warning) << "[" << subdomain << "] could not open/parse XML";
    VaeDbInternalError e;
    e.message = "Could not open/parse XML file";
    throw e;
  }
  xmlXPathOrderDocElems(doc);
  rootNode = rootDesignNode = NULL;
  Query _rootNodeQuery(this);
  _rootNodeQuery.runRawQuery(NULL, "/website/content", "website root node");
  if (_rootNodeQuery.getSize() != 1) {
    VaeDbInternalError e;
    e.message = "Could not find root content node in XML file";
    throw e;
  }
  rootNode = _rootNodeQuery.getNode(0);
  Query _rootDesignNodeQuery(this);
  _rootDesignNodeQuery.runRawQuery(NULL, "/website/design", "website design node");
  if (_rootDesignNodeQuery.getSize() != 1) {
    VaeDbInternalError e;
    e.message = "Could not find root design node in XML file";
    throw e;
  }
  rootDesignNode = _rootDesignNodeQuery.getNode(0);
  xmlNode *next;
  for (xmlNode *child = rootNode->children; child; child = next) {
    next = child->next;
    if (child->type == XML_ELEMENT_NODE) {
      Context *ctxt = new Context(this, child);
      if (ctxt->killMe) {
        xmlUnlinkNode(child);
        xmlFreeNode(child);
        delete ctxt;
      } else {
        child->_private = (void *)ctxt;
      }
    }
  }
  for (ContextList::iterator it = associationsToInitialize.begin(); it != associationsToInitialize.end(); it++) {
    (*it)->initializeAssociation();
  }
  associationsToInitialize.clear();
}

VaeDbStructure *Site::structureFromStructureId(int structureId) {
  StructureMap::const_iterator it;
  if ((it = structures.find(structureId)) != structures.end()) {
    return it->second;
  }
  stringstream q;
  q << "/website/design//*[@id=" << structureId << "]";
  Query _query(this);
  _query.runRawQuery(NULL, q.str(), q.str());
  if (!_query.getNode(0)) return NULL;
  VaeDbStructure *structure = new VaeDbStructure();
  structure->id = structureId;
  for (xmlAttr *child = _query.getNode(0)->properties; child; child = child->next) {
    if (!strcmp((const char *)child->name, "name")) structure->name = (const char *)child->children->content;
    if (!strcmp((const char *)child->name, "permalink")) structure->permalink = (const char *)child->children->content;
    if (!strcmp((const char *)child->name, "type")) structure->type = (const char *)child->children->content;
  }
  structures[structureId] = structure;
  return structure;
}
  
void Site::validateSecretKey(string testSecretKey) {
  if (!testMode && secretKey != testSecretKey) {
    L(warning) << "[" << subdomain << "] secret key mismatch: " << secretKey << " <> " << testSecretKey;
    VaeDbInternalError e;
    e.message = "Secret Key Mismatch";
    throw e;
  }
}

boost::shared_ptr<Query> Site::fetch_query(LRUKey const & key) {
  boost::shared_ptr<Query> * pp_query = query_cache.fetch_ptr(key, false);
  if(pp_query)
    return *pp_query;

  boost::shared_ptr<Query> p_query(new Query(this, key.p, key.s));
  query_cache.insert(key, p_query);
  return p_query;
}
