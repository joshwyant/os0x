#pragma once

#include "core/di.h"
#include "core/logging.h"
#include "core/runtime.h"
#include "core/status.h"
#include "core/stdlib/iostream.h"

namespace rtk {
class LogStream {
 public:
  LogStream(ostream& stream, StatusCode status)
      : stream_{stream}, status_{status} {}
  operator StatusCode() { return status_; }
  template <typename T>
  LogStream& operator<<(T& data) {
    stream_ << data;
    return *this;
  }

 private:
  ostream& stream_;
  StatusCode status_;
};

// To use this, declare `StatusCode status;` in scope, and return StatusCode
// as the return type.
// Example:
// StatusCode doSomething() {
//   StatusCode status;
//   CHECK_OK(something) << "Failed to do something!";
//   return Ok;
// }
// Output:
// CHECK failed: main.c:12: OutOfBounds (0x00000002): Access out of bounds. Failed to do something!
//
// DON'T do this:
// if (...)
//   CHECK_OK(something) << "...";
// else
//   ...
//
// Instead, use braces:
// if (...) {
//   CHECK_OK(something) << "...";
// } else {
//   ...
// }
// Otherwise, the macro will expand to something like this, which is WRONG:
// if (...)
//   status = (something);
//   if (status != Ok)
//     ... "CHECK_OK failed: " << "...";
// else
//   ...
// The first `if` only sets the status, and the `else` is dangling and
// matched not with the original condition, but with the status check.
#define _CHECK(name, result, fail_expression)                           \
  status = (result);                                                    \
  if (fail_expression)                                                  \
  return rtk::LogStream{rtk::GlobalContext().logger().stream(), status} \
         << name                                                        \
         " failed: " __FILE__                                           \
         ":" STRINGIZE(__LINE__) ": " << rtk::StatusString(status) << ". "

#define CHECK(expected, result) \
  _CHECK("CHECK", result, status != rtk::StatusCode::expected)
#define CHECK_FAILS(result) \
  _CHECK("CHECK_FAILS", result, status == rtk::StatusCode::Ok)
#define CHECK_OK(result) \
  _CHECK("CHECK_OK", result, status != rtk::StatusCode::Ok)
#define LOG_IF(expression)               \
  if (expression)                        \
  rtk::GlobalContext().logger().stream() \
      << __FILE__ ":" STRINGIZE(__LINE__) ": "

#ifndef STRINGIZE
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#endif  // ifndef STRINGIZE
}  // namespace rtk