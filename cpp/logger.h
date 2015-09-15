#include <iostream>
#include <sstream>

#include "memory_mgmt.h"

#define L(level) \
if (level > Logger::displayLevel) ; \
else Logger().log(level)

enum LogLevel {error, warning, info, debug};

class Logger : public gc {
 public:
  Logger();
  ~Logger();
  std::ostringstream& log(LogLevel level = info);
  LogLevel logLevel;
  static LogLevel displayLevel;
 protected:
  std::ostringstream os;
 private:
};
