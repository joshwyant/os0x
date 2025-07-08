#pragma once

#include <stddef.h>
#include "core/stdlib/cstring.h"

namespace rtk {
class string_view {
 public:
  constexpr string_view() : ptr_{nullptr}, len_{0} {}
  constexpr string_view(const char* str) : string_view{str, strlen(str)} {}
  constexpr string_view(const char* str, size_t len)
      : ptr_{str}, len_{strlen(str)} {}
  constexpr string_view(const string_view& other)
      : ptr_{other.ptr_}, len_{other.len_} {}
  constexpr const char* c_str() const { return ptr_; }
  constexpr size_t length() const { return len_; }

 private:
  const char* ptr_;
  const size_t len_;
};  // class string_view
}  // namespace rtk