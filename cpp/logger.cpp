using namespace std;

#include "logger.h"
#include <unistd.h>
#include <sys/syscall.h>

LogLevel Logger::displayLevel = info;

Logger::Logger() {
  os << "[" << syscall(SYS_gettid) << "] ";
}

Logger::~Logger() {
  cout << os.str() << endl;
  cout.flush();
}

ostringstream& Logger::log(LogLevel level) {
  logLevel = level;
  return os;
}
