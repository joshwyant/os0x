#include "core/format.h"
#include <stdarg.h>
#include <stdint.h>
using namespace rtk;
void Formatter::parsef(const char* format, ...) const {
  va_list arglist;

  // This is a varargs wrapper funtion. The va_list variant
  // does the heavy lifting. Other APIs can create their own
  // wrappers around vparsef in order to use parsef with
  // their own varargs functions.
  va_start(arglist, format);
  vparsef(format, arglist);
  va_end(arglist);
}

// https://cplusplus.com/reference/cstdio/printf/
void Formatter::vparsef(const char* format, va_list arglist) const {
  format_ = format;
  c_ = format;
  uintmax_t widened;
  intmax_t widenedSigned;

  while (*c_) {
    if (*c_ != '%') {
      // Just print the character.
      // Make a string to print
      char c[]{*(c_++), '\0'};
      outputChars(c);
    } else {
      c_++;
      parseFlags();
      if (parseWidth() == -1) {
        width_ = va_arg(arglist, int);
      }
      parsePrecision();
      parseLength();

      switch (*c_) {
        case 'd':
        case 'i':
          // signed decimal integer
          c_++;
          switch (size_) {
            case sizeof(int64_t):
              widenedSigned = va_arg(arglist, int64_t);
              break;
            case sizeof(int8_t):   // automatically promoted to int in varargs
            case sizeof(int16_t):  //
            case sizeof(int32_t):
            default:
              widenedSigned = static_cast<intmax_t>(va_arg(arglist, int32_t));
              break;
          }
          outputNumber(static_cast<uintmax_t>(widenedSigned), true, false, 10);
          break;
        case 'u':
          // unsigned decimal integer
        case 'o':
          // unsigned octal
        case 'x':
          // unsigned hexadecimal integer
        case 'X':
          // unsigned hexadecimal integer (uppercase)
        case 'p':
          // Pointer address
          switch (size_) {
            case sizeof(uint64_t):
              widened = va_arg(arglist, uint64_t);
              break;
            case sizeof(uint8_t):   // automatically promoted to int
            case sizeof(uint16_t):  //
            case sizeof(uint32_t):
            default:
              widened = static_cast<uintmax_t>(va_arg(arglist, uint32_t));
              break;
          }
          switch (*(c_++)) {
            case 'u':
              outputNumber(widened, false, false, 10);
              break;
            case 'o':
              outputNumber(widened, false, false, 8);
              break;
            case 'X':
              outputNumber(widened, false, true, 16);
              break;
            default:  // x, p
              outputNumber(widened, false, false, 16);
              break;
          }
          break;
        case 'f':
          // Decimal floating point, lowercase
        case 'e':
          // Scientific notation (mantissa/exponent), lowercase
        case 'E':
          // Scientific notation (mantissa/exponent), uppercase
        case 'g':
          // Use shortest representation: %e or %f
        case 'G':
          // Use shortest representation: %E or %F
        case 'a':
          // Hex floating point, lowercase
        case 'A':
          // Hex floating point, uppercase
          c_++;
          switch (size_) {
            case sizeof(long double):
              va_arg(arglist, long double);
              // Don't do anything: Unsupported
              break;
            default:  // double
              va_arg(arglist, double);
              // Don't do anything with it; floating point unsupported.
              break;
          }
          outputChars("?");
          break;
        case 'c':
          // Character
          c_++;
          switch (size_) {
            case sizeof(wchar_t): {
              auto _ = static_cast<wchar_t>(va_arg(arglist, uint32_t));
              // Don't do anything: Unsupported
              outputChars("?");
              break;
            }
            default:  // char
              char c = static_cast<char>(va_arg(arglist, uint32_t));
              char chrs[2]{c, '\0'};
              outputChars(chrs);
              break;
          }
          break;
        case 's':
          // String of characters
          c_++;
          switch (size_) {
            case sizeof(wchar_t): {
              auto _ = static_cast<wchar_t>(va_arg(arglist, uint32_t));
              // Don't do anything: Unsupported
              outputChars("?");
              break;
            }
            default:  // char
              const char* str = va_arg(arglist, const char*);
              outputChars(str);
              break;
          }
          break;
        case 'n':
          // Nothing printed.
          // The corresponding argument must be a pointer to a signed int.
          // The number of characters written so far is stored in the pointed location.
          c_++;
          switch (size_) {
            case sizeof(int64_t): {
              auto _ =
                  *va_arg(arglist,
                          int64_t*);  // = static_cast<int64_t>(charsPrinted_);
              break;
            }
            case sizeof(int8_t): {
              auto _ =
                  *va_arg(arglist,
                          int8_t*);  // = static_cast<int8_t>(charsPrinted_);
              break;
            }
            case sizeof(int16_t): {
              auto _ =
                  *va_arg(arglist,
                          int16_t*);  // = static_cast<int16_t>(charsPrinted_);
              break;
            }
            case sizeof(int32_t):
            default: {
              auto _ =
                  *va_arg(arglist,
                          int32_t*);  // = static_cast<int32_t>(charsPrinted_);
              break;
            }
          }
          break;

        case '%':
          // Escape for %
          outputChars(c_++);
          break;

        default:
          // What do we do here?
          // Maybe grab and discard the arg
          va_arg(arglist, int);
          outputChars("?");
          break;
      }
    }
  }
}

void Formatter::parseFlags() const {
  flags_ = FormatFlags::None;
  while (true) {
    switch (*c_) {
      case '-':
        flags_ |= FormatFlags::LeftJustify;
        c_++;
        break;
      case '+':
        flags_ |= FormatFlags::PlusPrefix;
        c_++;
        break;
      case ' ':
        flags_ |= FormatFlags::Space;
        c_++;
        break;
      case '#':
        flags_ |= FormatFlags::Hash;
        c_++;
        break;
      case '0':
        flags_ |= FormatFlags::Zero;
        c_++;
        break;
      default:
        return;
    }
  }
}

int Formatter::parseWidth() const {
  if (*c_ == '*') {
    c_++;
    return -1;  // get from vararg instead
  }

  width_ = 0;
  while (*c_ >= '0' && *c_ <= '9') {
    width_ = width_ * 10 + (*c_ - '0');
    c_++;
  }
  return width_;
}

void Formatter::parsePrecision() const {
  precision_ = 0;
  if (*c_ != '.') {
    return;
  }
  c_++;

  while (*c_ >= '0' && *c_ <= '9') {
    precision_ = precision_ * 10 + (*c_ - '0');
    c_++;
  }
}

void Formatter::parseLength() const {
  switch (*c_) {
    case 'h':
      c_++;
      if (*c_ == 'h') {
        c_++;
        size_ = sizeof(unsigned char);
      } else {
        size_ = sizeof(unsigned short int);
      }
      break;
    case 'l':
      c_++;
      if (*c_ == 'l') {
        c_++;
        size_ = sizeof(unsigned long long int);
      } else {
        size_ = sizeof(long);
      }
      break;
    case 'j':
      c_++;
      size_ = sizeof(uintmax_t);
      break;
    case 'z':
      c_++;
      size_ = sizeof(size_t);
      break;
    case 't':
      c_++;
      size_ = sizeof(ptrdiff_t);
      break;
    case 'L':
      c_++;
      size_ = sizeof(long double);
      break;
    default:
      size_ = 0;
      break;
  }
}

void Formatter::outputNumber(uintmax_t val, bool isSigned, bool upper,
                             int base) const {

  constexpr auto buffer_chrs = 20;
  char buffer[buffer_chrs + 1]{};
  char* str = buffer + buffer_chrs;
  int digits = 0;
  auto fillch = hasFlag(FormatFlags::Zero) ? "0" : " ";

  // Get the positive value of the number
  // and also track if signs add to the width
  intmax_t negative;
  auto positive = val;
  if (isSigned && (base == 10)) {
    negative = static_cast<intmax_t>(val);
    if (negative < 0) {
      positive = static_cast<uintmax_t>(-negative);
    }
  }

  // Get the string for the number portion
  if (base == 16) {
    if (upper) {
      outdigits<16, true>(val, digits, str);
    } else {
      outdigits<16, false>(val, digits, str);
    }
  } else if (base == 8) {
    outdigits<8>(val, digits, str);
  } else {
    outdigits<10>(positive, digits, str);
  }
  // Print the sign
  if (isSigned && (base == 10)) {
    if (negative < 0) {
      digits++;
      outputChars("-");
    } else if (hasFlag(FormatFlags::PlusPrefix)) {
      digits++;
      outputChars("+");
    } else if (hasFlag(FormatFlags::Space)) {
      digits++;
      outputChars(" ");
    }
  }
  // Count and print prefix
  if (hasFlag(FormatFlags::Hash)) {
    if (base == 8 && val == 0) {
      digits++;
      outputChars("0");
    } else if (base == 16) {
      digits += 2;
      outputChars(upper ? "0X" : "0x");
    }
  }
  // Left fill
  if (!hasFlag(FormatFlags::LeftJustify)) {
    while (digits < width_) {
      outputChars(fillch);
      digits++;
    }
  }
  // Print the number buffer
  outputChars(str);
  // right fill
  while (digits < width_) {
    outputChars(fillch);
    digits++;
  }
}

// void StringFormatter::outputChars(const char* chars, size_t length) const {}
