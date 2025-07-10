#pragma once

#include <stdint.h>

namespace rtk {
template <class T, T v>
// https://cplusplus.com/reference/type_traits/integral_constant/
struct integral_constant {
  static constexpr T value = v;
  typedef T value_type;
  typedef integral_constant<T, v> type;
  constexpr operator T() const noexcept { return v; }
  constexpr T operator()() const noexcept { return v; }
};

// https://cplusplus.com/reference/type_traits/true_type/
struct true_type : public integral_constant<bool, true> {};
struct false_type : public integral_constant<bool, false> {};

template <typename T1, typename T2>
struct is_same : public false_type {};

template <typename T>
struct is_same<T, T> : public true_type {};

template <typename T1, typename T2>
static constexpr bool is_same_v = is_same<T1, T2>::value;

template <typename Base, typename Derived>
struct is_inherited_from {
 private:
  static char test(Base*);
  static int test(...);

 public:
  static constexpr bool value =
      sizeof(test(static_cast<Derived*>(nullptr))) == sizeof(char);
};

template <typename Base, typename Derived>
static constexpr bool is_inherited_from_v =
    is_inherited_from<Base, Derived>::value;

template <typename T>
struct is_integral : public false_type {};

template <class T>
constexpr bool is_integral_v = is_integral<T>::value;

template <>
struct is_integral<uint8_t> : public true_type {};

template <>
struct is_integral<uint16_t> : public true_type {};

template <>
struct is_integral<uint32_t> : public true_type {};

template <>
struct is_integral<uint64_t> : public true_type {};

template <>
struct is_integral<int8_t> : public true_type {};

template <>
struct is_integral<int16_t> : public true_type {};

template <>
struct is_integral<int32_t> : public true_type {};

template <>
struct is_integral<int64_t> : public true_type {};

template <typename T>
struct is_signed : public false_type {};

template <class T>
static constexpr bool is_signed_v = is_signed<T>::value;

template <>
struct is_signed<int8_t> : public true_type {};

template <>
struct is_signed<int16_t> : public true_type {};

template <>
struct is_signed<int32_t> : public true_type {};

template <>
struct is_signed<int64_t> : public true_type {};

// template <>
// struct is_signed<uint8_t> : public false_type {};

// template <>
// struct is_signed<uint16_t> : public false_type {};

// template <>
// struct is_signed<uint32_t> : public false_type {};

// template <>
// struct is_signed<uint64_t> : public false_type {};

template <typename T>
struct is_unsigned : public false_type {};

template <class T>
static constexpr bool is_unsigned_v = is_unsigned<T>::value;

template <>
struct is_unsigned<uint8_t> : public true_type {};

template <>
struct is_unsigned<uint16_t> : public true_type {};

template <>
struct is_unsigned<uint32_t> : public true_type {};

template <>
struct is_unsigned<uint64_t> : public true_type {};

// template <>
// struct is_unsigned<int8_t> : public false_type {};

// template <>
// struct is_unsigned<int16_t> : public false_type {};

// template <>
// struct is_unsigned<int> : public false_type {};

// template <>
// struct is_unsigned<int64_t> : public false_type {};

template <bool, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> {
  typedef T type;
};

template <bool b, typename T = void>
using enable_if_t = typename enable_if<b, T>::type;

template <typename T>
struct add_const {
  typedef const T type;
};
template <typename T>
struct add_const<T*> {
  typedef const T* type;
};
template <typename T>
struct add_const<const T*> {
  typedef const T* type;
};
template <typename T>
struct add_const<T&> {
  typedef const T& type;
};
template <typename T>
struct add_const<T&&> {
  typedef const T& type;
};
template <typename T>
struct add_const<T (&)[]> {
  typedef const T (&type)[];
};
template <typename T>
using add_const_t = typename add_const<T>::type;

// https://en.cppreference.com/w/cpp/types/void_t.html
template <typename...>
using void_t = void;
}  // namespace rtk