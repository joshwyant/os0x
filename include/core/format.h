#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>

namespace rtk {
template <int base = 10, bool upper = false>
static inline void outdigits(uintmax_t value, int& digits, char*& buffer) {
  // Add digits in reverse
  while (value > 0) {
    const auto rem = value % base;
    auto& digit = *(--buffer);
    digits++;
    if constexpr (base == 16) {
      constexpr auto alphastart = upper ? 'A' : 'a';
      digit = rem < 10 ? '0' + rem : alphastart + (rem - 10);
    } else {
      digit = '0' + rem;
    }
    value /= base;
  }
  if (digits == 0) {
    *(--buffer) = '0';
    digits++;
  }
}

class Formatter {
 public:
  Formatter() {}  // : charsPrinted_{0} {}

  void vparsef(const char* format, va_list argp) const;
  void parsef(const char* format, ...) const;
  // int nparsef(const char* format, size_t count, ...) const;

 protected:
  // virtual size_t outputChars(const char* chars, size_t length) const = 0;
  virtual size_t outputChars(const char* chars) const = 0;
  //mutable size_t charsPrinted_;

 private:
  enum class FormatFlags;
  mutable const char* format_;
  mutable const char* c_;
  mutable FormatFlags flags_;
  mutable size_t size_;
  mutable int width_;
  mutable int precision_;
  void parseFlags() const;
  int parseWidth() const;
  void parsePrecision() const;
  void parseLength() const;
  void outputNumber(uintmax_t val, bool isSigned, bool upper, int base) const;
  enum class FormatFlags {
    None = 0,
    LeftJustify = 1,
    PlusPrefix = 1 << 1,
    Space = 1 << 2,
    Hash = 1 << 3,
    Zero = 1 << 4,
  };
  friend FormatFlags& operator|=(FormatFlags& a, FormatFlags b) {
    a = static_cast<FormatFlags>(static_cast<int>(a) | static_cast<int>(b));
    return a;
  }
  friend FormatFlags operator|(FormatFlags a, FormatFlags b) {
    return static_cast<FormatFlags>(static_cast<int>(a) | static_cast<int>(b));
  }
  bool hasFlag(FormatFlags mask) const {
    return (static_cast<int>(flags_) & static_cast<int>(mask)) != 0;
  }
};  // class Formatter

// class StringFormatter : public Formatter {
//  public:
//   StringFormatter(char* output) {}

//  protected:
//   size_t outputChars(const char* chars, size_t length) const override;
// };
}  // namespace rtk