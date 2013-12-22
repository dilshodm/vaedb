#ifndef _VAE_THRIFT_DB_HANDLER_H_
#define _VAE_THRIFT_DB_HANDLER_H_

#include <boost/thread/mutex.hpp>
#include "../gen-cpp/VaeDb.h"
#include "query_log.h"
#include "site.h"

typedef std::map<int,boost::shared_ptr<class Session> > SessionMap;
typedef std::map<std::string,boost::shared_ptr<class Site> > SiteMap;
typedef std::map<std::string,boost::mutex*> SiteMutexesMap;

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
  
  boost::shared_ptr<class Site> getSite(std::string subdomain, std::string secretKey, bool stagingMode);
  inline boost::shared_ptr<class Site> _getSite(std::string const & sitesKey, std::string const & secretKey);
  inline void _resetSite(std::string const & sitesKey, std::string const & secretKey, bool force);
  inline void _eraseSite(std::string const & sitesKey, std::string const & secretKey, bool force);

 public:
  VaeDbHandler(bool t, QueryLog & queryLog);
  ~VaeDbHandler();
  void closeSession(const int32_t sessionId, const std::string& secretKey);
  void createInfo(VaeDbCreateInfoResponse& _return, const int32_t session_id, const int32_t response_id, const std::string& query);
  void data(VaeDbDataResponse& _return, const int32_t session_id, const int32_t response_id);
  void get(VaeDbResponse& _return, const int32_t session_id, const int32_t  response_id, const std::string& query, const std::map<std::string, std::string> & options);
  SessionMap& getSessions();
  int32_t openSession(const std::string& subdomain, const std::string& secretKey, const bool stagingMode, const int32_t suggestedSessionId);
  int8_t ping();
  void resetSite(const std::string& subdomain, const std::string& secret_key);
  void reloadSite(std::string const & subdomain);
  void structure(VaeDbStructureResponse& _return, const int32_t session_id, const int32_t response_id);
  void writePid();
  
};

#endif
