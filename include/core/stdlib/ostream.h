#pragma once

#include <cstdint>
#include "core/stdlib/ios.h"
#include "core/stdlib/string_view.h"

namespace rtk {
class ostream : public ios {
 public:
  // virtual
  virtual ostream& operator<<(const char* data) = 0;
  virtual ostream& flush() = 0;
  // other basic types
  friend ostream& operator<<(ostream&, uintmax_t data);
  friend ostream& operator<<(ostream&, intmax_t data);
  friend ostream& operator<<(ostream&, bool data);
  // manipulators
  friend ostream& operator<<(ostream& s, ostream& (*manipulator)(ostream&)) {
    return manipulator(s);
  }
  friend ostream& operator<<(ostream& s, ios_base& (*manipulator)(ios_base&)) {
    return static_cast<ostream&>(manipulator(s));
  }
  friend ostream& operator<<(ostream& s, ios& (*manipulator)(ios&)) {
    return static_cast<ostream&>(manipulator(s));
  }
};  // class ostream
inline ostream& flush(ostream& str) {
  return str.flush();
}
inline ostream& endl(ostream& str) {
  return str << "\n" << flush;
}
}  // namespace rtk