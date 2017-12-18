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
typedef boost::function<json(boost::shared_ptr<class Session>, json&)> HandlerFunction;

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
  inline void _eraseSite(std::string const & sitesKey, std::string const & secretKey, bool force);
  inline void _resetSite(std::string const & sitesKey, std::string const & secretKey, bool force);
  inline boost::shared_ptr<class Site> _loadSite(std::string const & subdomain, bool stagingMode, std::string const & xml);
  inline boost::mutex & _get_site_mutex(std::string const & subdomain, bool stagingMode);

 public:
  boost::mutex sessionsMutex;

  Server(int workers, int port, QueryLog & queryLog, MemcacheProxy &memcacheProxy, MysqlProxy &mysqlProxy);
  ~Server();
  SessionMap& getSessions();
  string longTermKey(boost::shared_ptr<class Session>, string key);
  string shortTermKey(boost::shared_ptr<class Session>, string key);
  void process(string endpoint, bool requiresSession, const served::request &req, served::response &res, boost::function<json(boost::shared_ptr<class Session>, json&)> func);
  void reloadSite(std::string const & subdomain);
  void writePid();

  json closeSession(boost::shared_ptr<class Session>, json &params);
  json createInfo(boost::shared_ptr<class Session>, json &params);
  json data(boost::shared_ptr<class Session>, json &params);
  json get(boost::shared_ptr<class Session>, json &params);
  json longTermCacheGet(boost::shared_ptr<class Session>, json &params);
  json longTermCacheSet(boost::shared_ptr<class Session>, json &params);
  json longTermCacheEmpty(boost::shared_ptr<class Session>, json &params);
  json longTermCacheDelete(boost::shared_ptr<class Session>, json &params);
  json longTermCacheSweeperInfo(boost::shared_ptr<class Session>, json &params);
  json openSession(boost::shared_ptr<class Session>, json &params);
  json ping(boost::shared_ptr<class Session>, json &params);
  json sessionCacheGet(boost::shared_ptr<class Session>, json &params);
  json sessionCacheSet(boost::shared_ptr<class Session>, json &params);
  json sessionCacheDelete(boost::shared_ptr<class Session>, json &params);
  json shortTermCacheGet(boost::shared_ptr<class Session>, json &params);
  json shortTermCacheSet(boost::shared_ptr<class Session>, json &params);
  json shortTermCacheDelete(boost::shared_ptr<class Session>, json &params);
  json sitewideLock(boost::shared_ptr<class Session>, json &params);
  json sitewideUnlock(boost::shared_ptr<class Session>, json &params);
  json structure(boost::shared_ptr<class Session>, json &params);

};

#endif
