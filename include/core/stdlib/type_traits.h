#pragma once

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
struct is_same : false_type {};

template <typename T>
struct is_same<T, T> : true_type {};

// https://en.cppreference.com/w/cpp/types/void_t.html
template <typename...>
using void_t = void;
}  // namespace rtk