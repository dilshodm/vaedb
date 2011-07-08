#include <iostream>
#include <cstring>
#include <time.h>
#include <boost/smart_ptr.hpp>

#define COLLECTION_DESC "(collection)"

using namespace std;
using namespace boost;

#include "../gen-cpp/VaeDb.h"
#include "context.h"
#include "query.h"
#include "response.h"
#include "site.h"
#include "logger.h"

Context::Context(Site *s, const xmlNodePtr n) : site(s) { 
  node = n;
  id = 0;
  dataPopulated = false;
  permalink = NULL;
  structure = NULL;
  killMe = false;
  if (populateAttrs()) {
    killMe = true;
    return;
  }
  if (type == "AssociationItem" || type == "SingleAssociationItem") {
    site->associationsToInitialize.push_back(this);
  } else {  
    xmlNode *next;
    for (xmlNode *child = node->children; child; child = next) {
      next = child->next;
      if (child->type == XML_ELEMENT_NODE) {      
        if (child->_private == NULL) {
          Context *ctxt = new Context(site, child);
          if (ctxt->killMe) {
            xmlUnlinkNode(child);
            xmlFreeNode(child);
            delete ctxt;
          } else {
            child->_private = (void *)ctxt;
          }
        }
      }
    }
  }
  site->nodes[id] = node;
  vaeDbContext.id = id;
  if (structure) {
    vaeDbContext.structure_id = structure->id;
    vaeDbContext.type = type;
  }
  if (permalink) {
    vaeDbContext.permalink = permalink;
    site->permalinks[permalink] = node;
  } else {
    vaeDbContext.permalink = "";
  }
  if (id == 0) {
    vaeDbContext.data = getSingleData();
  }
}

int Context::getCount(const string &query) {
  boost::unique_lock<boost::mutex> lock(mutex);
  CachedCountMap::const_iterator it;
  if ((it = cachedCounts.find(query)) != cachedCounts.end()) {
    return it->second;
  }
  Query _query(site, this, query);
  int result = _query.getSize();  
  cachedCounts[query] = result;
  return result;
}

string Context::getData(const string &query) {
  if (!dataPopulated) populateData();
  boost::unique_lock<boost::mutex> lock(mutex);
  if (query == "") {
    throw VaeDbInternalError("Empty query passed to Context::getData");
  }
  DataMap::const_iterator it;
  if ((it = data.find(query)) != data.end()) {
    return it->second;
  }
  if ((it = cachedQueries.find(query)) != cachedQueries.end()) {
    return it->second;
  }
  Query _query(site, this, query);
  string result = _query.getSingleData();  
  cachedQueries[query] = result;
  return result;
}

int32_t Context::getId() {
  return id;
}

const char *Context::getNodeName() {
  return (const char *)node->name;
}

string Context::getSingleData() {
  xmlNode *child = node->children;
  if (child && (child->type == XML_TEXT_NODE)) return (const char *)child->content;
  return "";
}

int32_t Context::getStructureId() {
  return structure->id;
}

void Context::initializeAssociation() {
  Query _query(site);
  int id = atoi(getSingleData().c_str());
  if (id > 0) {
    _query.find(id);
    if (_query.getSize() == 1) {
    
      /* set up forward association */
      xmlNodePtr associatedNode = _query.getNode(0);
      xmlFreePropList(node->properties);
      site->nodesToClean2.push_back(node);
      if (node->children) xmlFreeNodeList(node->children->next);
      node->children->next = associatedNode->children;
      node->properties = associatedNode->properties;
      node->last = associatedNode->last;
      node->_private = associatedNode->_private;
    
      /* set up reverse association */
      xmlNodePtr pointerNode = xmlCopyNode(node->parent, 0);
      if (pointerNode == NULL) throw VaeDbInternalError("xmlCopyNode returned NULL");
      pointerNode->_private = node->parent->_private;
      pointerNode->children = node->parent->children;
      pointerNode->last = node->parent->last;
      pointerNode->properties = node->parent->properties;
      site->nodesToClean.push_back(pointerNode);
      xmlAddChild(associatedNode, pointerNode);
    
      /* update previously placed pointer nodes to include this stuff */
      Context *parentContext = (Context *)node->parent->_private;
      Context *associatedContext = (Context *)associatedNode->_private;
      parentContext->pointerNodes.push_back(pointerNode);
      for (NodeList::iterator it = associatedContext->pointerNodes.begin(); it != associatedContext->pointerNodes.end(); it++) {
        (*it)->last = associatedNode->last;
      }
      
      /* this context is no longer eneded */
      delete this;
      return;
    }
  }
  
  /* couldn't find association */
  xmlUnlinkNode(node);
  xmlFreeNode(node);
  delete this;
}

bool Context::populateAttrs() {
  int structureId = 0;
  for (xmlAttr *child = node->properties; child; child = child->next) {
    if (!strcmp((const char *)child->name, "id")) id = atoi((const char *)child->children->content);
    if (!strcmp((const char *)child->name, "type")) structureId = atoi((const char *)child->children->content);
    if (!strcmp((const char *)child->name, "permalink")) permalink = (char *)child->children->content;
    if (!site->stagingMode && !strcmp((const char *)child->name, "staging")) {
      return true;
    }
  }
  if (structureId > 0) {
    structure = site->structureFromStructureId(structureId);
    if (structure) type = structure->type;
  }
  return false;
}

void Context::populateData() {
  boost::unique_lock<boost::mutex> lock(mutex);
  if (dataPopulated) return;
  for (xmlNode *child = node->children; child; child = child->next) {
    if (child->type == XML_ELEMENT_NODE) {      
      const char *content = NULL;
      for (xmlNode *child2 = child->children; child2; child2 = child2->next) {
        if (child2->type == XML_TEXT_NODE) {
          content = (const char *)child2->content;
        } else {
          content = COLLECTION_DESC;
          break;
        }
      }
      if (content != NULL && child->name) {
        data[(const char *)child->name] = content;
      }
    }
  }
  dataPopulated = true;
}

VaeDbContext Context::toVaeDbContext() {
  return vaeDbContext;
}

VaeDbDataForContext Context::toVaeDbDataForContext() {
  VaeDbDataForContext _return;
  if (!dataPopulated) populateData();
  _return.data = data;
  return _return;
}

VaeDbStructure Context::toVaeDbStructureForContext() {
  if (structure) {
    return *structure;
  } else {
    return VaeDbStructure();
  }
}
