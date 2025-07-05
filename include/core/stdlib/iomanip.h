#pragma once

#include <cstddef>
#include <iomanip>
#include "core/stdlib/ostream.h"

namespace rtk {
struct _Setw {
  int n_;
};
static inline _Setw setw(int n) {
  return _Setw{n};
}
ostream& operator<<(ostream& str, _Setw&& _setw) {
  str.width(_setw.n_);
  return str;
}

struct _Setiosflags {
  ios_base::fmtflags f_;
};
static inline _Setiosflags setiosflags(ios_base::fmtflags f) {
  return _Setiosflags{f};
}
ostream& operator<<(ostream& str, _Setiosflags&& _setiosflags) {
  str.setf(_setiosflags.f_);
  return str;
}

struct _Resetiosflags {
  ios_base::fmtflags f_;
};
static inline _Resetiosflags resetiosflags(ios_base::fmtflags f) {
  return _Resetiosflags{f};
}
ostream& operator<<(ostream& str, _Resetiosflags&& _resetiosflags) {
  str.unsetf(_resetiosflags.f_);
  return str;
}

struct _Setfill {
  char c_;
};
static inline _Setfill setfill(char c) {
  return _Setfill{c};
}
ostream& operator<<(ostream& str, _Setfill&& _setfill) {
  str.fill(_setfill.c_);
  return str;
}

struct _Setbase {
  int b_;
};
static constexpr inline _Setbase setbase(int b) {
  return _Setbase{b};
}
ostream& operator<<(ostream& str, _Setbase&& _setbase) {
  switch (_setbase.b_) {
    case 10:
      return str << rtk::dec;
    case 16:
      return str << rtk::hex;
    case 8:
      return str << rtk::oct;
    default:
      return str;  // TODO: reset?
  }
}

}  // namespace rtk