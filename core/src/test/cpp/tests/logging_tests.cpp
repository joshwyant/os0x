#include <iostream>
#include "core/logging.h"
#include "test/test.h"

std::ostream& operator<<(std::ostream& s, rtk::LogLevel lvl) {
  switch (lvl) {
#define X(name, _)                        \
  case rtk::LogLevel::name:               \
    std::cout << "rtk::LogLevel::" #name; \
    break;
    LOGLEVEL_LIST
#undef X
  }
  return s;
}

namespace rtk {
class LoggingTests {
 public:
  static int core_test_log_levels() {
    EXPECT_LESS_THAN(LogLevel::Fatal, LogLevel::Error);
    EXPECT_LESS_THAN(LogLevel::Error, LogLevel::Info);
    EXPECT_LESS_THAN(LogLevel::Info, LogLevel::Warn);
    EXPECT_LESS_THAN(LogLevel::Warn, LogLevel::Debug);
    EXPECT_LESS_THAN(LogLevel::Debug, LogLevel::Trace);
    return 0;
  }
  static void core_logging_tests() { TEST(core_test_log_levels); }

};  // class LoggingTests
}  // namespace rtk

void core_logging_tests() {
  rtk::LoggingTests::core_logging_tests();
}