#include <boost/thread/mutex.hpp>

#include "memory_mgmt.h"

class Session : public gc {

  time_t createdAt;
  vector< boost::shared_ptr<class Response> > responses;
  boost::shared_ptr<class Site> site;
  
 public:
  Session(boost::shared_ptr<class Site> s);
  void createInfo(VaeDbCreateInfoResponse& _return, const int32_t responseId, const string& query);
  void data(VaeDbDataResponse& _return, const int32_t responseId);
  void get(VaeDbResponse &_return, const int32_t responseId, const string &query, const map<string, string> &options);
  time_t getCreatedAt();
  boost::shared_ptr<class Site> getSite();
  void structure(VaeDbStructureResponse& _return, const int32_t responseId);

};
