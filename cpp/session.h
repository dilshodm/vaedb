#include <boost/thread/mutex.hpp>

class Session {

  time_t createdAt;
  vector< shared_ptr<class Response> > responses;
  shared_ptr<class Site> site;
  
 public:
  Session(shared_ptr<class Site> s);
  void createInfo(VerbDbCreateInfoResponse& _return, const int32_t responseId, const string& query);
  void data(VerbDbDataResponse& _return, const int32_t responseId);
  void get(VerbDbResponse &_return, const int32_t responseId, const string &query, const map<string, string> &options);
  time_t getCreatedAt();
  shared_ptr<class Site> getSite();
  void structure(VerbDbStructureResponse& _return, const int32_t responseId);

};
