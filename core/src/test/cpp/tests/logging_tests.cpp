#include <cstdarg>
#include <iostream>
#include "core/format.h"
#include "core/logging.h"
#include "core/stdlib/ostream.h"
#include "core/stdlib/sstream.h"
#include "test/test.h"

namespace rtk {
// For test output
std::ostream& operator<<(std::ostream& s, rtk::LogLevel lvl) {
  switch (lvl) {
#define X(name, _)                        \
  case rtk::LogLevel::name:               \
    std::cout << "rtk::LogLevel::" #name; \
    break;
    LOGLEVEL_LIST
#undef X
    default:
      std::cout << "rtk::LogLevel::None";
      break;
  }
  return s;
}
class FakeFormatter : public Formatter {
 public:
  FakeFormatter(ostream& stream) : stream_{stream} {}

 protected:
  size_t outputChars(const char* chars) const override {
    auto chrs = rtk::strlen(chars);
    stream_.write(chars);
    //charsPrinted_ += chrs;
    return chrs;
  }

 private:
  rtk::ostream& stream_;
};

class FakeLogger : public Logger {
 public:
  FakeLogger() : Logger(rtk::LogLevel::Debug) {}
  FakeLogger(LogLevel level) : Logger(level) {}
  const char* contents() { return stream_.str().c_str(); }
  ostream& stream() const override { return stream_; }
  void vlog(const string_view format, va_list argp) const override {
    FakeFormatter formatter{stream()};
    formatter.vparsef(format.c_str(), argp);
  }
  void clear() { stream_.str().clear(); }

 private:
  mutable ostringstream stream_;
};
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
  static int core_test_log_stream() {
    FakeLogger logger;
    logger.stream() << "Hello, world!" << "\n";

    EXPECT_EQUAL(logger.contents(), "Hello, world!\n");

    return 0;
  }
  static int core_test_log_format() {
    FakeLogger logger;
    logger.debug("Hello, %s!", "world");
    EXPECT_EQUAL(logger.contents(), "Hello, world!");

    logger.clear();
    logger.debug("Number %d", 100);
    EXPECT_EQUAL(logger.contents(), "Number 100");
    return 0;
  }
  static void core_logging_tests() {
    TEST(core_test_log_levels);
    TEST(core_test_log_stream);
    TEST(core_test_log_format);
  }

};  // class LoggingTests
}  // namespace rtk

void core_logging_tests() {
  rtk::LoggingTests::core_logging_tests();
}