#pragma once

// Only works with std::. Try using <initializer_list> instead of this hack
namespace std {
template <typename T>
class initializer_list {
 public:
  using value_type = T;
  using reference = const T&;
  using const_reference = const T&;
  using size_type = size_t;
  using iterator = const T*;
  using const_iterator = const T*;

  constexpr initializer_list() noexcept : begin_(nullptr), size_(0) {}

  constexpr initializer_list(const T* begin, size_t size) noexcept
      : begin_(begin), size_(size) {}

  constexpr const T* begin() const noexcept { return begin_; }
  constexpr const T* end() const noexcept { return begin_ + size_; }
  constexpr size_t size() const noexcept { return size_; }

 private:
  const T* begin_;
  size_t size_;
};  // class initializer_list
}  // namespace std