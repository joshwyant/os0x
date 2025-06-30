#pragma once

namespace k {
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

template <typename>
struct void_t {}
}  // namespace k