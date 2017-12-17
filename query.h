#ifndef _VAE_THRIFT_QUERY_H_
#define _VAE_THRIFT_QUERY_H_

#include <boost/thread/mutex.hpp>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

class Query {

  class Site *site;
  int size;
  xmlXPathObjectPtr xpathObj;
  static boost::mutex mutex;

 public:
  Query(Site *s);
  Query(class Site *s, Context *context, const string &q);
  ~Query();
  void find(int id);
  xmlNodePtr getNode(int i);
  const char *getSingleData();
  int getSize();
  void runDesignQuery(Context *context, const string &q);
  void runQuery(Context *context, const string &q, const string &displayQuery);
  void runRawQuery(xmlNodePtr node, const string &q, const string &displayQuery);
  bool singleData();

 private:
  void result(xmlNode *node);

};

#endif
