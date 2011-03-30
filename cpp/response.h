#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

typedef vector<class Context *> ContextList;
struct ResponseContext {
  ContextList contexts;
  int32_t total;
};
typedef vector<ResponseContext> ResponseContextList;
typedef vector<VaeDbCreateInfo> CreateInfoList;
  
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
  shared_ptr<Site> site;
  vector<SortField> sortFields;
  SortType sortType;
  int skip;
  bool unique;
  vector<string> uniqueFields;
  set<string> uniqueFound;
  
 public:
  Response(shared_ptr<Site> s, shared_ptr<Response> parent, const string &q, const map<string, string> &options);
  Response(shared_ptr<Site> s, shared_ptr<Response> parent, const string &query);
  ~Response();
  bool containsContexts();
  bool sortCallback(Context *lhs, Context *rhs);
  void writeVaeDbCreateInfoResponse(VaeDbCreateInfoResponse &_return);
  void writeVaeDbDataResponse(VaeDbDataResponse &_return);
  void writeVaeDbResponse(VaeDbResponse &_return);
  void writeVaeDbStructureResponse(VaeDbStructureResponse &_return);

 private:
  void createInfo(Context *context, const string &query);
  bool filterMatch(xmlNode *node);
  void filterMatchRecursive(xmlNode *node, int reentry);
  void query(Context *context, const string &q, const map<string, string> &options);
  bool uniqueMatch(xmlNode *node);

};
