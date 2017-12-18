#ifndef _VAE_THRIFT_DB_HANDLER_H_
#define _VAE_THRIFT_DB_HANDLER_H_

#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <served/served.hpp>
#include "memcache_proxy.h"
#include "mysql_proxy.h"
#include "query_log.h"
#include "site.h"

typedef std::map<int,boost::shared_ptr<class Session> > SessionMap;
typedef std::map<std::string,boost::shared_ptr<class Site> > SiteMap;
typedef std::map<std::string,boost::mutex*> SiteMutexesMap;

extern int testMode;

class Server {

 private:
  int nextSessionId;
  SessionMap sessions;
  SiteMap sites;
  SiteMutexesMap siteMutexes;
  boost::mutex sitesMutex;
  QueryLog &queryLog;
  MemcacheProxy &memcacheProxy;
  MysqlProxy &mysqlProxy;

  boost::shared_ptr<class Site> getSite(std::string subdomain, std::string secretKey, bool stagingMode);
  inline boost::shared_ptr<class Site> _getSite(std::string const & sitesKey, std::string const & secretKey);
  inline void _resetSite(std::string const & sitesKey, std::string const & secretKey, bool force);
  inline void _eraseSite(std::string const & sitesKey, std::string const & secretKey, bool force);
  inline boost::shared_ptr<class Site> _loadSite(std::string const & subdomain, bool stagingMode, std::string const & xml);
  inline boost::mutex & _get_site_mutex(std::string const & subdomain, bool stagingMode);

 public:
  boost::mutex sessionsMutex;

  Server(int workers, int port, QueryLog & queryLog, MemcacheProxy &memcacheProxy, MysqlProxy &mysqlProxy);
  ~Server();
  void process(string endpoint, const served::request &req, served::response &res, boost::function<json(json)> func);
  SessionMap& getSessions();
  void reloadSite(std::string const & subdomain);
  void writePid();

  json closeSession(const int32_t sessionId, const std::string& secretKey);
  json createInfo(const int32_t session_id, const int32_t response_id, const std::string& query);
  json data(const int32_t session_id, const int32_t response_id);
  json get(const int32_t session_id, const int32_t  response_id, const std::string& query, const std::map<std::string, std::string> & options);
  json openSession(const std::string& subdomain, const std::string& secretKey, const bool stagingMode, const int32_t suggestedSessionId);
  json ping(json params);
  json resetSite(const std::string& subdomain, const std::string& secret_key);
  json structure(const int32_t session_id, const int32_t response_id);

  json shortTermCacheGet(json params);
  json shortTermCacheSet(json params);
  json shortTermCacheDelete(json params);
  json sessionCacheGet(const int32_t sessionId, string const & key);
  json sessionCacheSet(const int32_t sessionId, string const & key, string const & value);
  json sessionCacheDelete(const int32_t sessionId, string const & key);
  json longTermCacheGet(const int32_t sessionId, string const & key, const int32_t renewExpiry, const int32_t useShortTermCache);
  json longTermCacheSet(const int32_t sessionId, string const & key, string const & value, const int32_t expireInterval, const int32_t isFilename);
  json longTermCacheEmpty(const int32_t sessionId);
  json longTermCacheDelete(const int32_t sessionId, string const & key);
  json longTermCacheSweeperInfo(const int32_t session_id);
  json sitewideLock(const int32_t sessionId, string const & iden);
  json sitewideUnlock(const int32_t sessionId, string const & iden);

};

#endif
