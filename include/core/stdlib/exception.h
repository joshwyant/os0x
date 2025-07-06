#pragma once

namespace rtk {
// Only use within traps in contexts where exceptions are supported!
// E.g. unit testing
class exception {
 public:
  virtual ~exception() = default;
  virtual const char* what() const { return "rtk::exception"; };
};  // class exception
}  // namespace rtk