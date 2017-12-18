#include <boost/thread/mutex.hpp>

class Session {

  time_t createdAt;
  vector< boost::shared_ptr<class Response> > responses;
  boost::shared_ptr<class Site> site;

 public:
  Session(boost::shared_ptr<class Site> s);
  json createInfo(const int32_t responseId, const string& query);
  json data(const int32_t responseId);
  json get(const int32_t responseId, const string &query, const map<string, string> &options);
  time_t getCreatedAt();
  boost::shared_ptr<class Site> getSite();
  json structure(const int32_t responseId);

};
