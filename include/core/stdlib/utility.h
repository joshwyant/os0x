#pragma once

namespace rtk {

template <typename T>
struct remove_reference {
  using type = T;
};
template <typename T>
struct remove_reference<T&> {
  using type = T;
};
template <typename T>
struct remove_reference<T&&> {
  using type = T;
};

template <typename T>
inline typename remove_reference<T>::type&& move(T&& a) {
  // accepts lvalue or rvalue, returns rvalue
  // - T&& is forwarding reference, not rvalue like int&&
  //     - int&& if T is int
  //     - int& if T is int&
  //     - int&& if T is int&&
  // - remove_reference<T>::type&& is unconditional rvalue:
  //     - int&& if T is int
  //     - int&& if T is int&
  //     - int&& if T is int&&
  // - T is int:   (int&&) -> (int&&) - input is rvalue or rvalue reference
  // - T is int&:  (int&) -> (int&&)  - input is lvalue reference
  // - T is int&&: (int&&) -> (int&&) - input is rvalue reference
  return static_cast<typename remove_reference<T>::type&&>(a);
}
template <typename T>
inline void swap(T& a, T& b) noexcept {
  T temp = move(a);  // a: empty, b: original b, temp: original a
  a = move(b);       // a: original b, b: empty, temp: original a
  b = move(temp);    // a: original b, b: original a, temp: empty
  // temp goes out of scope and is either empty or a copy and deconstructed
}
template <class T>
const T& as_const(T& t) noexcept {
  return static_cast<const T&>(t);
}

template <typename T>
T&& forward(typename remove_reference<T>::type& t) {
  // Accepts lvalue, returns original lvalue or rvalue preserved
  // - returns T&& is forwarding reference, not rvalue like int&&
  //     - int&& if T is int
  //     - int& if T is int&
  //     - int&& if T is int&&
  // - remove_reference<T>::type& is unconditional lvalue:
  //     - int& if T is int
  //     - int& if T is int&
  //     - int& if T is int&&
  // - T is int:   input must be lvalue or lvalue reference: (int&) -> (int&&)
  // - T is int&:  input is lvalue or lvalue reference:      (int&) -> (int&)
  // - T is int&&: input is lvalue, lvalue reference,
  //               or rvalue reference                       (int&) -> (int&&)
  // if T is consistent with input:
  // - T is int:   input is lvalue int:             (int&) -> (int&&)
  // - T is int&:  input is lvalue reference int&:  (int&) -> (int&)
  // - T is int&&: input is rvalue reference int&&: (int&) -> (int&&)
  return static_cast<T&&>(t);
}
}  // namespace rtk