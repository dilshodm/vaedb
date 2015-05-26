#define THREADED

#include <iostream>
#include <fstream>
#include <signal.h>
#include <execinfo.h>
#include <boost/program_options.hpp> 
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <malloc.h>

using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace boost;
using namespace std;
namespace po = boost::program_options;

#include "../gen-cpp/VaeDb.h"
#include "site.h"
#include "context.h"
#include "logger.h"
#include "response.h"
#include "s3.h"
#include "session.h"
#include "vae_db_handler.h"
#include "bus.h"

void crash_handler(int signal) {
  const int MAX_STACK_DEPTH = 100;

  void *stack_entries[MAX_STACK_DEPTH];
  size_t size = backtrace(stack_entries, MAX_STACK_DEPTH);

  cerr << "caught signal " << signal << endl;
  backtrace_symbols_fd(stack_entries, size, 1);

  if(signal == SIGSEGV)
    exit(-1);
}

int main(int argc, char **argv) {

  signal(SIGSEGV, crash_handler);
  signal(SIGUSR1, crash_handler);

  //abuse of libxml internal pointers results in doublefree;
#ifdef M_CHECK_ACTION
  mallopt(M_CHECK_ACTION, 0);
#endif

  int opt, workers;
  string bus_bindaddress;
  char * _access_key = getenv("AWS_ACCESS_KEY");
  char * _secret_key = getenv("AWS_SECRET_KEY");

  string aws_access_key(_access_key ? _access_key : "");
  string aws_secret_key(_secret_key ? _secret_key : "");
  string aws_bucket;
  string feed_cache_path;
  string mysql_username, mysql_password, mysql_database, mysql_host, memcached_host;

  po::options_description desc("vaedb options", 80);
  desc.add_options()
    ("help,H", "outputs this help message")
    ("log_level,L", po::value<string>(), "log level.  Acceptable values: error, warning, info, debug")
    ("query_log,Q", po::value<string>(), "file to log queries to")
    ("port,P", po::value<int>(&opt)->default_value(9091), "port to run the server on")
    ("busaddress,B", po::value<string>(&bus_bindaddress)->default_value("tcp://*:5091"), "bind address for the zmq subscriber")
    ("aws_access_key,A", po::value<string>(&aws_access_key)->default_value(aws_access_key), "AWS access key")
    ("aws_secret_key,S", po::value<string>(&aws_secret_key)->default_value(aws_secret_key), "AWS secret key")
    ("aws_bucket,U", po::value<string>(&aws_bucket)->default_value(aws_bucket), "AWS bucket")
    ("feed_cache_path", po::value<string>(&feed_cache_path)->default_value("/tmp"), "feed cache path") 
    ("workers,w", po::value<int>(&workers)->default_value(12), "number of worker threads to spawn")
    ("mysql_username", po::value<string>(&mysql_username), "MySQL username")
    ("mysql_password", po::value<string>(&mysql_password), "MySQL password")
    ("mysql_database", po::value<string>(&mysql_database), "MySQL database name")
    ("mysql_host", po::value<string>(&mysql_host), "MySQL hostname or IP Address")
    ("memcached_host", po::value<string>(&memcached_host), "Memcached hostname or IP Address")
  ;
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }
  if (vm.count("log_level")) {
    if (vm["log_level"].as<string>() == "error") Logger::displayLevel = error;
    else if (vm["log_level"].as<string>() == "warning") Logger::displayLevel = warning;
    else if (vm["log_level"].as<string>() == "info") Logger::displayLevel = info;
    else if (vm["log_level"].as<string>() == "debug") Logger::displayLevel = debug;
    else L(warning) << "Invalid log_level specified: '" << vm["log_level"].as<string>() << "', defaulting to 'warning'.";
  }

  auto_ptr<ostream> p_querylog_stream;
  if(vm.count("query_log")) {
    p_querylog_stream.reset(new ofstream(vm["query_log"].as<string>().c_str()));
    if(p_querylog_stream->fail()) {
      L(warning) << "Unable to write to query log file: '" << vm["query_log"].as<string>() << "'.";
      p_querylog_stream.reset();
    }
  }
  
  QueryLog query_log(p_querylog_stream.get());

  if(!initialize_s3(aws_access_key, aws_secret_key, aws_bucket, feed_cache_path)) {
    L(error) << "S3 failed to initialize.";
    return -1;
  }

  boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  boost::shared_ptr<VaeDbHandler> handler(new VaeDbHandler(query_log));
  boost::shared_ptr<TProcessor> processor(new VaeDbProcessor(handler));
  boost::shared_ptr<TServerSocket> serverSocket(new TServerSocket(vm["port"].as<int>()));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

#ifdef THREADED 
  boost::shared_ptr<Bus> bus(new Bus(handler, vm["busaddress"].as<string>()));
  threadFactory->newThread(bus)->start();

  boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(workers);
  threadManager->threadFactory(threadFactory);
  threadManager->start();
  boost::shared_ptr<TServer> server(new TThreadPoolServer(processor, serverSocket, transportFactory, protocolFactory, threadManager));
#else
  boost::shared_ptr<TServer> server(new TSimpleServer(processor, serverSocket, transportFactory, protocolFactory));
#endif

  server->serve();
  
  return 0;
}
