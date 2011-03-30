#include <boost/thread/mutex.hpp>

typedef map<int,shared_ptr<class Session> > SessionMap;
typedef map<string,shared_ptr<class Site> > SiteMap;
typedef map<string,boost::mutex*> SiteMutexesMap;
  
class VerbDbHandler : virtual public VerbDbIf {

 private:
  bool testMode;
  int nextSessionId;
  SessionMap sessions;
  boost::mutex sessionsMutex;
  SiteMap sites;
  SiteMutexesMap siteMutexes;
  boost::mutex sitesMutex;
  
  shared_ptr<class Site> getSite(string subdomain, string secretKey, bool stagingMode);
  
 public:
  VerbDbHandler(bool t);
  ~VerbDbHandler();
  void closeSession(const int32_t sessionId, const std::string& secretKey);
  void createInfo(VerbDbCreateInfoResponse& _return, const int32_t session_id, const int32_t response_id, const std::string& query);
  void data(VerbDbDataResponse& _return, const int32_t session_id, const int32_t response_id);
  void get(VerbDbResponse& _return, const int32_t session_id, const int32_t  response_id, const std::string& query, const std::map<std::string, std::string> & options);
  SessionMap& getSessions();
  int32_t openSession(const std::string& subdomain, const std::string& secretKey, const bool stagingMode);
  int8_t ping();
  void resetSite(const std::string& subdomain, const std::string& secret_key);
  void structure(VerbDbStructureResponse& _return, const int32_t session_id, const int32_t response_id);
  void writePid();
  
};
