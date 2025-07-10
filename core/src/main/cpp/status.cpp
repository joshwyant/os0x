#include "core/status.h"
#include "core/logging.h"
// #include "core/runtime.h"

using namespace rtk;

void rtk::StatusTrap(StatusCode status) {
  // GlobalContext().logger().stream()
  //     << "Error trapped: " << StatusString(status);
}

void rtk::StatusTrap(StatusCode status, string_view file, int line) {
  // GlobalContext().logger().stream()
  //     << file.c_str() << ":" << line
  //     << ": Error trapped: " << StatusString(status);
}