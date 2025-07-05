#include "core/status.h"
// #include "core/runtime.h"

using namespace rtk;

void rtk::StatusTrap(StatusCode status) {
  // GlobalContext().logger().stream()
  //     << "Error trapped: " << StatusString(status);
  // constexpr auto t = StatusString(StatusCode::OutOfBounds);
}

void rtk::StatusTrap(StatusCode status, string_view file, int line) {
  // GlobalContext().logger().stream()
  //     << file << ":" << line << ": Error trapped: " << StatusString(status);
  // constexpr auto t = StatusString(StatusCode::OutOfBounds);
}