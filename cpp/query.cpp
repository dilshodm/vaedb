#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>
#include <pcrecpp.h>

using namespace boost;
using namespace std;

#include "../gen-cpp/VaeDb.h"
#include "context.h"
#include "site.h"
#include "query.h"
#include "logger.h"

const char EMPTY_STRING[] = "";

boost::mutex Query::mutex;

Query::Query(Site *s) : site(s), xpathObj(NULL) {
}

Query::Query(Site *s, Context *context, const string &q) : site(s), xpathObj(NULL) {
  string id, permalink, query = q;
  if (pcrecpp::RE("^/").Replace("", &query)) {
    context = NULL;
  }
  if (pcrecpp::RE("permalink/(.*)").FullMatch(query, &permalink)) {
    vector<string> pathParts;
    boost::split(pathParts, permalink, boost::is_any_of("/"));
    vector<string>::iterator end = pathParts.end();
    for (int i = pathParts.size(); i >= 0; i--) {
      if (i == 0) {
        result(NULL);
        return;
      }
      vector<string> subPathParts(pathParts.begin(), end);
      permalink = boost::join(subPathParts, "/");
      if (site->permalinks.count(permalink)) {
        if (i == pathParts.size()) {
          result(site->permalinks[permalink]);
          return;
        }
        context = (Context *)site->permalinks[permalink]->_private;
        vector<string> queryPathParts(end, pathParts.end());
        query = boost::join(queryPathParts, "/");
        break;
      }
      end--;
    }
  }
  if (pcrecpp::RE("([0-9]+)(/.*|)").FullMatch(query, &id, &query)) {
    find(atoi(id.c_str()));
    if (size && (query.length() > 0)) {
      context = (Context *)getNode(0)->_private;
      if (xpathObj != NULL) {
        xmlXPathFreeObject(xpathObj);
      }
      xpathObj = NULL;
      query = query.substr(1);
    } else {
      return;
    }
  }

  // prev() and next()
  if (!strcasecmp("prev()", query.c_str())) {
    if (!context || !context->getNodeName()) {
      VaeDbQueryError e;
      e.message = "You cannot use PREV() outside of a context.";
      throw e;
    }
    query = (string)"preceding-sibling::" + context->getNodeName() + "[1]";
  }
  if (!strcasecmp("next()", query.c_str())) {
    if (!context || !context->getNodeName() {
      VaeDbQueryError e;
      e.message = "You cannot use NEXT() outside of a context.";
      throw e;
    }
    query = (string)"following-sibling::" + context->getNodeName() + "[1]";
  }

  // hack in a /text() to the end of predicate selectors
  vector<string> queryParts;
  boost::split(queryParts, query, boost::is_any_of("'"));
  for (int i = 0; i < queryParts.size(); i += 2) {
    while (pcrecpp::RE("/([0-9]+)").Replace("[@id='\\1']", &queryParts[i])) { }
    while (pcrecpp::RE("\\[([_a-z])([a-z0-9_/]+)([^(a-z0-9_/])").Replace("[\\1\\2/text()\\3", &queryParts[i])) { }
    while (pcrecpp::RE(" (or|and) ([_a-z])([a-z0-9_/]+)([^(a-z0-9_/])").Replace(" \\1 \\2\\3/text()\\4", &queryParts[i])) { }
    while (pcrecpp::RE("([^@])([_a-z])([0-9a-z_/]+) (or|and) ").Replace("\\1\\2\\3/text() \\4 ", &queryParts[i])) { }
    while (pcrecpp::RE("([^@])([_a-z])([0-9a-z_/]+)\\]").Replace("\\1\\2\\3/text()]", &queryParts[i])) { }
    if (strstr(queryParts[i].c_str(), "//")) {
      VaeDbQueryError e;
      e.message = "Double-slashes are not supported in Vae queries.";
      throw e;
    }
  }
  query = boost::join(queryParts, "'");

  // hack of the century.  without this, complex lookups fail
  // I haven't the faintest idea why.  But this does seem to fix it.
  if (strstr(query.c_str(), "/") && !strstr(query.c_str(), "[")) {
    boost::split(queryParts, query, boost::is_any_of("/"));
    if (!strstr(queryParts[0].c_str(), ".")) {
      queryParts[0] += "[@id!='']";
      query = boost::join(queryParts, "/");
    }
  }

  runQuery(context, query, q);
}

Query::~Query() {
  if (xpathObj != NULL) {
    xmlXPathFreeObject(xpathObj);
  }
}

void Query::find(int id) {
  if (site->nodes.count(id)) {
    result(site->nodes[id]);
  } else {
    result(NULL);
  }
}

xmlNodePtr Query::getNode(int i) {
  return xpathObj->nodesetval->nodeTab[i];
}

const char *Query::getSingleData() {
  if (getSize() > 0) {
    xmlNode *child = xpathObj->nodesetval->nodeTab[0]->children;
    if (child && (child->type == XML_TEXT_NODE)) return (const char *)child->content;
  }
  return EMPTY_STRING;
}

int Query::getSize() {
  return size;
}

void Query::result(xmlNode *node) {
  xmlNodeSetPtr nodeset = (xmlNodeSet *)malloc(sizeof(xmlNodeSet));
  if (nodeset == NULL) {
    L(error) << "malloc fail";
    abort();
  }
  xpathObj = (xmlXPathObject *)malloc(sizeof(xmlXPathObject));
  if (xpathObj == NULL) {
    L(error) << "malloc fail";
    abort();
  }
  xpathObj->type = XPATH_NODESET;
  xpathObj->boolval = 0;
  nodeset->nodeTab = (xmlNodePtr *)malloc(sizeof(xmlNodePtr));
  if (nodeset->nodeTab == NULL) {
    L(error) << "malloc fail";
    abort();
  }
  xpathObj->nodesetval = nodeset;
  if (node) {
    nodeset->nodeNr = nodeset->nodeMax = size = 1;
    nodeset->nodeTab[0] = node;
  } else {
    nodeset->nodeNr = nodeset->nodeMax = size = 0;
  }
}

void Query::runDesignQuery(Context *context, const string &query) {
  xmlNodePtr node;
  if (context) {
    stringstream q;
    q << "/website/design//*[@id=" << context->getStructureId() << "]";
    Query _query(site);
    _query.runRawQuery(NULL, q.str(), q.str());
    node = _query.getNode(0);
  } else {
    node = site->rootDesignNode;
  }
  runRawQuery(node, query, query);
}

void Query::runQuery(Context *context, const string &q, const string &displayQuery) {
  xmlNodePtr node;
  if (context) {
    node = context->node;
    L(debug) << " looking under context " << context->getId();
  } else {
    node = site->rootNode;
  }
  runRawQuery(node, q, displayQuery);
}

void Query::runRawQuery(xmlNodePtr node, const string &q, const string &displayQuery) {
  xmlXPathContext *xpathContext = xmlXPathNewContext(site->doc);

  if (xpathContext == NULL) {
    L(warning) << "[" << site->getSubdomain() << "] could not create libxml2 XPath Context";
    VaeDbInternalError e;
    e.message = "could not create libxml2 XPath Context";
    throw e;
  }
  xpathContext->node = node;
  xpathObj = xmlXPathEvalExpression((xmlChar *)q.c_str(), xpathContext);
  xmlXPathFreeContext(xpathContext);
  if (xpathObj == NULL) {
    L(warning) << "[" << site->getSubdomain() << "] could not evaluate XPath expression: '" << displayQuery << "', transformed into '" << q << "'";
    string error = "could not evaluate XPath expression: '" + displayQuery + "'";
    VaeDbQueryError e;
    e.message = error.c_str();
    throw e;
  }
  if (xpathObj->type != XPATH_NODESET) {
    L(warning) << "[" << site->getSubdomain() << "] libxml2 returned a strange response: '" << xpathObj->type << "'.  Query was '" << q << "'";
    VaeDbInternalError e;
    e.message = "libxml2 returned something other than a nodeset?!";
    throw e;
  }
  size = (xpathObj->nodesetval) ? xpathObj->nodesetval->nodeNr : 0;
  if (size && (getNode(0) == site->rootNode)) {
    xmlXPathFreeObject(xpathObj);
    xpathObj=NULL;
    result(NULL);
  }
}

bool Query::singleData() {
  bool single = (size == 1);
  if (single) {
    for (xmlAttr *child = xpathObj->nodesetval->nodeTab[0]->properties; child; child = child->next) {
      if (!strcmp((const char *)child->name, "id")) {
        single = false;
        break;
      }
    }
  }
  return single;
}
