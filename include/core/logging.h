#pragma once

#include <cstdarg>
#include "core/stdlib/iostream.h"
#include "core/stdlib/string.h"

#define LOGLEVEL_LIST \
  X(Fatal, fatal)     \
  X(Error, error)     \
  X(Info, info)       \
  X(Warn, warn)       \
  X(Debug, debug)     \
  X(Trace, trace)

namespace rtk {
enum class LogLevel {
  None,
#define X(name, _) name,
  LOGLEVEL_LIST
#undef X
};  // enum class LogLevel
inline bool operator<=(LogLevel a, LogLevel b) {
  return static_cast<int>(a) <= static_cast<int>(b);
}
inline bool operator>(LogLevel a, LogLevel b) {
  return static_cast<int>(a) > static_cast<int>(b);
}

class Logger {
 public:
  Logger(LogLevel level) : level_{level} {}
  virtual ~Logger() noexcept = 0;
  // Implementation free to prefix timestamp, etc.
  virtual void vlog(const string_view format, va_list argp) const = 0;
  virtual ostream& stream() = 0;
  void log(const string_view format, ...) {
    va_list arglist;
    va_start(arglist, format);
    vlog(format, arglist);
    va_end(arglist);
  }
  void log(LogLevel level, const string_view format, ...) const {
    va_list arglist;
    va_start(arglist, format);
    vlog(level, format, arglist);
    va_end(arglist);
  }
  void vlog(LogLevel level, const string_view format, va_list argp) const {
    if (level > level_)
      return;
    vlog(format, argp);
  }
  template <typename F>
  void log(LogLevel level, F msgFunc) {
    if (level <= level_)
      log(msgFunc());
  }
  LogLevel level() const { return level_; }
  void setLevel(LogLevel level) { level_ = level; }
#define X(level, fnName)                                         \
  void v##fnName(const string_view format, va_list argp) const { \
    vlog(LogLevel::level, format, argp);                         \
  }                                                              \
  void fnName(const string_view format, ...) {                   \
    va_list arglist;                                             \
    va_start(arglist, format);                                   \
    v##fnName(format, arglist);                                  \
    va_end(arglist);                                             \
  }
  LOGLEVEL_LIST
#undef X

 private:
  LogLevel level_;
};  // class Logger

inline Logger::~Logger() noexcept {}

}  // namespace rtk

#ifndef STRINGIZE
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#endif  // ifndef

#define LOGGER GlobalContext().logger()
#define PRINT(format, ...) LOGGER.log(format __VA_OPT__(, ) __VA_ARGS__)
#define _LOG(label, level, format, ...)           \
  LOGGER.log(rtk::LogLevel::level, label __FILE__ \
             ":" STRINGIZE(__LINE__) ": " format __VA_OPT__(, ) __VA_ARGS__)
#define FATAL(format, ...) \
  _LOG("[FATAL] ", Fatal, format __VA_OPT__(, ) __VA_ARGS__)
#define ERROR(format, ...) \
  _LOG("[ERROR] ", Error, format __VA_OPT__(, ) __VA_ARGS__)
#define INFO(format, ...) \
  _LOG("[INFO]  ", Info, format __VA_OPT__(, ) __VA_ARGS__)
#define WARN(format, ...) \
  _LOG("[WARN]  ", Warn, format __VA_OPT__(, ) __VA_ARGS__)
#define DEBUG(format, ...) \
  _LOG("[DEBUG] ", Debug, format __VA_OPT__(, ) __VA_ARGS__)
#define TRACE(format, ...) \
  _LOG("[TRACE] ", Trace, format __VA_OPT__(, ) __VA_ARGS__)