#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <set>

typedef vector<class Context *> ContextList;
struct ResponseContext {
  ContextList contexts;
  int32_t total;
};
typedef struct ResponseContext ResponseContext;
struct CreateInfo {
  int32_t structure_id;
  int32_t row_id;
};
typedef struct CreateInfo CreateInfo;
typedef vector<ResponseContext> ResponseContextList;
typedef vector<CreateInfo> CreateInfoList;

class Response {

  enum SortType {None, Reverse, Shuffle, Specific};
  enum SortOrder {Asc, Desc};
  struct SortField {
    string field;
    bool count;
    SortOrder direction;
  };

  ResponseContextList contexts;
  CreateInfoList createInfos;
  vector<string> data;
  bool filter;
  vector<char *> filterTerms;
  vector<char *> filterRemainingTerms;
  int groups;
  int page;
  int paginate;
  boost::shared_ptr<Site> site;
  vector<SortField> sortFields;
  SortType sortType;
  int skip;
  bool unique;
  vector<string> uniqueFields;
  set<string> uniqueFound;

 public:
  Response(boost::shared_ptr<Site> s, boost::shared_ptr<Response> parent, const string &q, const map<string, string> &options);
  Response(boost::shared_ptr<Site> s, boost::shared_ptr<Response> parent, const string &query);
  ~Response();
  bool containsContexts();
  bool sortCallback(Context *lhs, Context *rhs);
  json getCreateInfo();
  json getData();
  json getJson();
  json getStructure();

 private:
  void createInfo(Context *context, const string &query);
  bool filterMatch(xmlNode *node);
  void filterMatchRecursive(xmlNode *node, int reentry);
  void query(Context *context, const string &q, const map<string, string> &options);
  bool uniqueMatch(xmlNode *node);

};
