#pragma once

#include <stdint.h>
#include "core/stdlib/ios.h"
#include "core/stdlib/string.h"
#include "core/stdlib/string_view.h"
#include "core/stdlib/type_traits.h"

namespace rtk {

template <typename T>
struct make_cstr {};

template <>
struct make_cstr<const char*> {
  typedef const char* type;
  static constexpr const char* get(const char* str) noexcept {
    return static_cast<const char*>(str);
  }
};
template <>
struct make_cstr<const char[]> {
  typedef const char type[];
  static constexpr const char* get(const char str[]) noexcept {
    return static_cast<const char*>(str);
  }
};
template <>
struct make_cstr<const char (&)[]> {
  typedef const char (&type)[];
  static constexpr const char* get(const char (&str)[]) noexcept {
    return static_cast<const char*>(str);
  }
};
template <>
struct make_cstr<const string> {
  typedef const string type;
  static const char* get(const string str) noexcept;  // { return str.c_str(); }
};
template <>
struct make_cstr<const string&> {
  typedef const string& type;
  static constexpr const char* get(const string& str) noexcept {
    return str.c_str();
  }
};
template <>
struct make_cstr<const string&&> {
  typedef const string&& type;
  static constexpr const char* get(const string&& str) noexcept {
    return str.c_str();
  }
};
template <>
struct make_cstr<const string*> {
  typedef const string* type;
  static constexpr const char* get(const string* str) noexcept {
    return str->c_str();
  }
};

template <typename T>
struct is_string : false_type {};
template <typename T>
static constexpr bool is_string_v = is_string<T>::value;
template <>
struct is_string<const char*> : true_type {};
template <>
struct is_string<char*> : true_type {};
template <>
struct is_string<const char (&)[]> : true_type {};
template <>
struct is_string<char (&)[]> : true_type {};
template <>
struct is_string<string_view&&> : true_type {};
template <>
struct is_string<string_view&> : true_type {};
template <>
struct is_string<string_view*> : true_type {};
template <>
struct is_string<string&&> : true_type {};
template <>
struct is_string<string&> : true_type {};
template <>
struct is_string<string*> : true_type {};
class ostream : public ios {
 private:
 public:
  // virtual
  virtual ostream& write(const char* cstr) = 0;
  virtual ostream& flush() = 0;
  // non-virtual
  ostream& write(uintmax_t data, bool is_signed);
  // template <typename T, typename>
  // friend ostream& operator<<(ostream& s, T data);
  // streaming operators
  template <typename T>
  friend enable_if_t<is_string_v<T>, ostream&> operator<<(ostream& s, T data) {
    return s.write(make_cstr<add_const_t<T>>::get(data));
  }
  // Signed integral types
  template <typename T>
  friend enable_if_t<is_signed_v<T>, ostream&> operator<<(ostream& s, T data) {
    return s.write(static_cast<uintmax_t>(data), true);
  }
  // // Unsigned integral types
  template <typename T>
  friend enable_if_t<is_unsigned_v<T>, ostream&> operator<<(ostream& s,
                                                            T data) {
    return s.write(static_cast<uintmax_t>(data), false);
  }
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