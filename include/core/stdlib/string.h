#pragma once

#include <cstddef>
#include "core/stdlib/cstring.h"
#include "core/stdlib/freestanding/new.h"  // placement new
#include "core/stdlib/memory.h"
#include "core/stdlib/string_view.h"
#include "core/stdlib/utility.h"

namespace rtk {
class string;
class string_view;
constexpr size_t kMaxShortLen = 23;

class string {
  union {
    struct {  // 8 + 16 bytes
      bool is_short_ : 1;
      size_t len_ : 63;
      unique_ptr<char[]> cstr_;
    };
    // For short buffer optimization
    struct {  // 1 + 23 bytes
      bool is_short1_ : 1;
      size_t shortlen_ : 7;
      char shortstr_[kMaxShortLen];
    };
  };

 public:
  string() : is_short1_{true}, shortstr_{}, shortlen_{0} {}
  string(const char* cstr) : string() {
    size_t len = rtk::strlen(cstr);
    copy_cstr(cstr, len);
  }
  string(string_view& sv) : string() { copy_cstr(sv.c_str(), sv.length()); }
  string(const string& other) : string() { *this = other; }
  string(string&& other) noexcept : string() { swap(*this, other); }
  string& operator=(const string& other) {
    if (this == &other)
      return *this;
    copy_cstr(other.c_str(), other.length());
    return *this;
  }
  string& operator=(string&& other) noexcept {
    if (this == &other)
      return *this;
    if (other.is_short_) {
      copy_cstr(other.c_str(), other.length());
    } else {
      is_short_ = false;
      len_ = other.len_;
      cstr_ = move(other.cstr_);
    }
    other.reset();
    return *this;
  }
  friend void swap(string& a, string& b) {
    // Use move assign manually to guarantee behavior
    string temp = move(a);
    a = move(b);
    b = move(temp);
  }
  constexpr operator string_view() const { return {c_str(), length()}; }
  const char* c_str() const { return is_short_ ? shortstr_ : cstr_.get(); }
  char* c_str() { return const_cast<char*>(as_const(*this).c_str()); }
  size_t length() const { return is_short_ ? shortlen_ : len_; }
  virtual ~string() { reset(); }

  string& operator+=(string_view other) { return append(other); }
  string operator+(string_view other) {
    string start;
    auto len = length();
    auto otherlen = other.length();
    auto newlen = len + otherlen;
    if (newlen < kMaxShortLen) {
      start.append(*this);
      start.append(other);
      return start;
    }
    char* ptr = new char[newlen + 1];
    strncpy(ptr, c_str(), len);
    strncpy(ptr + len, other.c_str(), otherlen + 1);
    start.is_short_ = false;
    start.len_ = newlen;
    start.cstr_ = {ptr, newlen + 1};
    return start;
  }

  string& append(string_view other) {
    const auto len = length();
    const auto otherlen = other.length();
    const auto newlen = len + otherlen;
    const auto need_buffer = len + otherlen > kMaxShortLen - 1;
    // Create a new buffer if necessary
    char* dest = need_buffer ? new char[newlen + 1] : shortstr_;
    // Copy A to the buffer.
    if (need_buffer) {
      strncpy(dest, c_str(), len);
    }
    // Copy B.
    strncpy(dest + len, other.c_str(), otherlen + 1);
    // Release old buffer
    if (need_buffer) {
      if (is_short_) {
        // Un-corrupt
        new (&cstr_) unique_ptr<char[]>{};
      }
      cstr_ = {dest, newlen + 1};
    }
    // Set properties
    is_short_ = !need_buffer;
    if (need_buffer) {
      len_ = newlen;
    } else {
      shortlen_ = newlen;
    }

    return *this;
  }

 private:
  friend class string_tests;
  void reset() {
    if (!is_short_) {
      cstr_.reset();
      is_short_ = true;
    }
    shortlen_ = 0;
    strncpy(shortstr_, "", kMaxShortLen);  // does zero fill
  }
  void copy_cstr(const char* original,
                 size_t len) {  // we already have length
    reset();
    if (len >= kMaxShortLen) {
      auto arraylen = len + 1;
      unique_ptr<char[]> newptr(arraylen);
      if (newptr) {
        is_short_ = false;
        len_ = len;
        cstr_ = move(newptr);
      }
    } else {
      is_short_ = true;
      shortlen_ = len;
    }
    // Use strncpy for safety
    rtk::strncpy(c_str(), original, len);
    // Ensure null terminated after strncpy for safety
    (c_str())[len] = '\0';
  }
};  // class string
}  // namespace rtk