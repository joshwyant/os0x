#pragma once

#include <cstddef>
#include <cstdint>

namespace rtk {
enum class _Fmtflags : uintptr_t {
  none = 0,
  boolalpha = 0x0001,
  showbase = 0x0002,
  showpoint = 0x0004,
  showpos = 0x0008,
  skipws = 0x0010,
  unitbuf = 0x0020,
  uppercase = 0x0040,
  dec = 0x0080,
  hex = 0x0100,
  oct = 0x0200,
  fixed = 0x0400,
  scientific = 0x0800,
  internal = 0x1000,
  left = 0x2000,
  right = 0x4000,
};  // enum class _Fmtflags

constexpr _Fmtflags operator|(_Fmtflags a, _Fmtflags b) {
  return static_cast<_Fmtflags>(static_cast<uintptr_t>(a) |
                                static_cast<uintptr_t>(b));
}
constexpr _Fmtflags& operator|=(_Fmtflags& a, _Fmtflags b) {
  return a = static_cast<_Fmtflags>(static_cast<uintptr_t>(a) |
                                    static_cast<uintptr_t>(b));
}
constexpr _Fmtflags operator&(_Fmtflags a, _Fmtflags b) {
  return static_cast<_Fmtflags>(static_cast<uintptr_t>(a) &
                                static_cast<uintptr_t>(b));
}
constexpr _Fmtflags& operator&=(_Fmtflags& a, _Fmtflags b) {
  return a = static_cast<_Fmtflags>(static_cast<uintptr_t>(a) &
                                    static_cast<uintptr_t>(b));
}
constexpr _Fmtflags operator^(_Fmtflags a, _Fmtflags b) {
  return static_cast<_Fmtflags>(static_cast<uintptr_t>(a) ^
                                static_cast<uintptr_t>(b));
}
constexpr _Fmtflags& operator^=(_Fmtflags& a, _Fmtflags b) {
  return a = static_cast<_Fmtflags>(static_cast<uintptr_t>(a) ^
                                    static_cast<uintptr_t>(b));
}
constexpr _Fmtflags operator~(_Fmtflags val) {
  return static_cast<_Fmtflags>(~static_cast<uintptr_t>(val));
}
constexpr bool operator!(_Fmtflags val) {
  return val == _Fmtflags::none;
}
constexpr bool operator==(_Fmtflags a, int b) {
  return static_cast<int>(a) == b;
}
constexpr bool operator!=(_Fmtflags a, int b) {
  return static_cast<int>(a) != b;
}
class ios_base {
 public:
  using fmtflags = _Fmtflags;

 protected:
  size_t width_ = 0;
  fmtflags flags_ = fmtflags::none;

 public:
  static constexpr fmtflags boolalpha = fmtflags::boolalpha;
  static constexpr fmtflags showbase = fmtflags::showbase;
  static constexpr fmtflags showpoint = fmtflags::showpoint;
  static constexpr fmtflags showpos = fmtflags::showpos;
  static constexpr fmtflags skipws = fmtflags::skipws;
  static constexpr fmtflags unitbuf = fmtflags::unitbuf;
  static constexpr fmtflags uppercase = fmtflags::uppercase;
  static constexpr fmtflags dec = fmtflags::dec;
  static constexpr fmtflags hex = fmtflags::hex;
  static constexpr fmtflags oct = fmtflags::oct;
  static constexpr fmtflags fixed = fmtflags::fixed;
  static constexpr fmtflags scientific = fmtflags::scientific;
  static constexpr fmtflags internal = fmtflags::internal;
  static constexpr fmtflags left = fmtflags::left;
  static constexpr fmtflags right = fmtflags::right;
  fmtflags flags() const { return flags_; }
  fmtflags setf(fmtflags f) { return rtk::operator|=(flags_, f); }
  fmtflags setf(fmtflags f, fmtflags mask) {
    return flags_ = (flags_ & ~mask) | (f & mask);
  }
  fmtflags unsetf(fmtflags f) { return flags_ &= ~f; }
  size_t width() const { return width_; }
  size_t width(size_t w) { return width_ = w; }

  // accepts a function pointer for an e.g.
  //   ios_base& boolalpha(ios_base& str) {...} function.
  // Usage:
  //   out << boolalpha << ...;
  // Same as:
  //   rtk::boolalpha(out) << ...;
  ios_base& operator<<(ios_base& (*fn)(ios_base&)) { return fn(*this); }
};  // class ios_base
class ios : public ios_base {
 public:
  char fill() const { return fill_; }
  char fill(char fillch) { return fill_ = fillch; }

 protected:
  char fill_ = ' ';
};

inline ios_base& boolalpha(ios_base& str) {
  str.setf(_Fmtflags::boolalpha);
  return str;
}
inline ios_base& noboolalpha(ios_base& str) {
  str.unsetf(_Fmtflags::boolalpha);
  return str;
}

inline ios_base& showbase(ios_base& str) {
  str.setf(_Fmtflags::showbase);
  return str;
}
inline ios_base& noshowbase(ios_base& str) {
  str.unsetf(_Fmtflags::showbase);
  return str;
}

inline ios_base& showpoint(ios_base& str) {
  str.setf(_Fmtflags::showpoint);
  return str;
}
inline ios_base& noshowpoint(ios_base& str) {
  str.unsetf(_Fmtflags::showpoint);
  return str;
}

inline ios_base& showpos(ios_base& str) {
  str.setf(_Fmtflags::showpos);
  return str;
}
inline ios_base& noshowpos(ios_base& str) {
  str.unsetf(_Fmtflags::showpos);
  return str;
}

inline ios_base& skipws(ios_base& str) {
  str.setf(_Fmtflags::skipws);
  return str;
}
inline ios_base& noskipws(ios_base& str) {
  str.unsetf(_Fmtflags::skipws);
  return str;
}

inline ios_base& unitbuf(ios_base& str) {
  str.setf(_Fmtflags::unitbuf);
  return str;
}
inline ios_base& nounitbuf(ios_base& str) {
  str.unsetf(_Fmtflags::unitbuf);
  return str;
}

inline ios_base& uppercase(ios_base& str) {
  str.setf(_Fmtflags::uppercase);
  return str;
}
inline ios_base& nouppercase(ios_base& str) {
  str.unsetf(_Fmtflags::uppercase);
  return str;
}
inline ios_base& dec(ios_base& str) {
  str.unsetf(_Fmtflags::hex | _Fmtflags::oct);
  str.setf(_Fmtflags::dec);
  return str;
}
inline ios_base& hex(ios_base& str) {
  str.unsetf(_Fmtflags::dec | _Fmtflags::oct);
  str.setf(_Fmtflags::hex);
  return str;
}
inline ios_base& oct(ios_base& str) {
  str.unsetf(_Fmtflags::hex | _Fmtflags::dec);
  str.setf(_Fmtflags::oct);
  return str;
}
inline ios_base& fixed(ios_base& str) {
  str.unsetf(_Fmtflags::scientific);
  str.setf(_Fmtflags::fixed);
  return str;
}
inline ios_base& scientific(ios_base& str) {
  str.unsetf(_Fmtflags::fixed);
  str.setf(_Fmtflags::scientific);
  return str;
}
inline ios_base& internal(ios_base& str) {
  str.unsetf(_Fmtflags::left | _Fmtflags::right);
  str.setf(_Fmtflags::internal);
  return str;
}
inline ios_base& left(ios_base& str) {
  str.unsetf(_Fmtflags::internal | _Fmtflags::right);
  str.setf(_Fmtflags::left);
  return str;
}
inline ios_base& right(ios_base& str) {
  str.unsetf(_Fmtflags::left | _Fmtflags::internal);
  str.setf(_Fmtflags::right);
  return str;
}

}  // namespace rtk