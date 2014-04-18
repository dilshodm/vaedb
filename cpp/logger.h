#include <iostream>
#include <sstream>

#define L(level) \
if (level > Logger::displayLevel) ; \
else Logger().log(level)

enum LogLevel {error, warning, info, debug};

class Logger {
 public:
  Logger();
  ~Logger();
  ostringstream& log(LogLevel level = info);
  LogLevel logLevel;
  static LogLevel displayLevel;
 protected:
  ostringstream os;
 private:
};
