using namespace std;

#include "logger.h"

LogLevel Logger::displayLevel = info;

Logger::~Logger() {
  cout << os.str() << endl;
  cout.flush();
}

ostringstream& Logger::log(LogLevel level) {
  logLevel = level;
  return os;
}
