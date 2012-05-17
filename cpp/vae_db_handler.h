#include <boost/thread/mutex.hpp>
#include "query_log.h"

typedef map<int,shared_ptr<class Session> > SessionMap;
typedef map<string,shared_ptr<class Site> > SiteMap;
typedef map<string,boost::mutex*> SiteMutexesMap;

class VaeDbHandler : virtual public VaeDbIf {

 private:
  bool testMode;
  int nextSessionId;
  SessionMap sessions;
  boost::mutex sessionsMutex;
  SiteMap sites;
  SiteMutexesMap siteMutexes;
  boost::mutex sitesMutex;
  QueryLog & queryLog;
  
  shared_ptr<class Site> getSite(string subdomain, string secretKey, bool stagingMode);

 public:
  VaeDbHandler(bool t, QueryLog & queryLog);
  ~VaeDbHandler();
  void closeSession(const int32_t sessionId, const std::string& secretKey);
  void createInfo(VaeDbCreateInfoResponse& _return, const int32_t session_id, const int32_t response_id, const std::string& query);
  void data(VaeDbDataResponse& _return, const int32_t session_id, const int32_t response_id);
  void get(VaeDbResponse& _return, const int32_t session_id, const int32_t  response_id, const std::string& query, const std::map<std::string, std::string> & options);
  SessionMap& getSessions();
  int32_t openSession(const std::string& subdomain, const std::string& secretKey, const bool stagingMode);
  int8_t ping();
  void resetSite(const std::string& subdomain, const std::string& secret_key);
  void structure(VaeDbStructureResponse& _return, const int32_t session_id, const int32_t response_id);
  void writePid();
  
};
