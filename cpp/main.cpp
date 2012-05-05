#define THREADED

#include <iostream>
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
#include "session.h"
#include "vae_db_handler.h"

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

  int opt, workers;
  po::options_description desc("vaedb options", 80);
  desc.add_options()
    ("help,H", "outputs this help message")
    ("log_level,L", po::value<string>(), "log level.  Acceptable values: error, warning, info, debug")
    ("port,P", po::value<int>(&opt)->default_value(9091), "port to run the server on")
    ("test,T", "runs the server in test mode, which allows clients to specify full XML paths and does not validate secret keys.  It is insecure to run a server in test mode on a production machine!")
    ("workers,w", po::value<int>(&workers)->default_value(12), "number of worker threads to spawn")
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
  
  shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  shared_ptr<VaeDbHandler> handler(new VaeDbHandler(vm.count("test")));
  shared_ptr<TProcessor> processor(new VaeDbProcessor(handler));
  shared_ptr<TServerSocket> serverSocket(new TServerSocket(vm["port"].as<int>()));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

#ifdef THREADED  
  shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(workers);
  threadManager->threadFactory(threadFactory);
  threadManager->start();
  shared_ptr<TServer> server(new TThreadPoolServer(processor, serverSocket, transportFactory, protocolFactory, threadManager));
#else
  shared_ptr<TServer> server(new TSimpleServer(processor, serverSocket, transportFactory, protocolFactory));
#endif

  server->serve();
  
  return 0;
}
