#pragma once

#include <cstdint>
#include "core/stdlib/ios.h"
#include "core/stdlib/string_view.h"

namespace rtk {
class ostream : public ios {
 public:
  // virtual
  virtual ostream& operator<<(string_view data) = 0;
  virtual ostream& flush() = 0;
  // other basic types
  ostream& operator<<(uintmax_t data);
  ostream& operator<<(intmax_t data);
  ostream& operator<<(bool data);
  ostream& operator<<(const char* data) {
    return operator<<(string_view{data});
  }
  // manipulators
  ostream& operator<<(ostream& (*manipulator)(ostream&)) {
    return manipulator(*this);
  }
  ostream& operator<<(ios_base& (*manipulator)(ios_base&)) {
    return static_cast<ostream&>(manipulator(*this));
  }
  ostream& operator<<(ios& (*manipulator)(ios&)) {
    return static_cast<ostream&>(manipulator(*this));
  }
};  // class ostream
inline ostream& flush(ostream& str) {
  return str.flush();
}
inline ostream& endl(ostream& str) {
  return str << "\n" << flush;
}
}  // namespace rtk