#include "core/stdlib/ostream.h"
#include "core/stdlib/string.h"

using namespace rtk;

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

static inline void out(ostream& stream, uintmax_t value,
                       ios_base::fmtflags flags, size_t width, bool isSigned,
                       bool negative, char fill) {
  const auto showsign = isSigned && negative || ((flags & ios::showpos) != 0);
  const auto upper = (flags & ios::uppercase) != 0;
  const auto hex = (flags & ios::hex) != 0;
  const auto octal = (flags & ios::oct) != 0;
  const auto left = (flags & ios::left) != 0;
  const auto internal = (flags & ios::internal) != 0;
  const auto showbase = (flags & ios::showbase) != 0;

  constexpr auto buf_chrs = 20;  // 20 is good for 64 bits
  char buffer[buf_chrs + 1]{/*zeros*/};
  char* str = buffer + buf_chrs;  // go in reverse
  auto printed = 0;
  if (hex) {
    if (upper) {
      outdigits<16, true>(value, printed, str);
    } else {
      outdigits<16, false>(value, printed, str);
    }
  } else if (octal) {
    outdigits<8>(value, printed, str);
  } else {
    outdigits(value, printed, str);
  }

  // Prepare to print
  const char fillstr[] = {fill,
                          '\0'};  // string representing the fill character
  if (showsign) {
    printed++;  // include in count before printing padding
  }
  if ((hex || octal) && showbase) {
    printed += hex ? 2
               : value != 0
                   ? 1
                   : 0;  // for octal, prepend extra 0 if value itself is not 0
  }
  // pad left
  if (left) {
    for (; printed < width; printed++) {
      stream << (const char*)fillstr;
    }
  }
  // Print sign
  if (showsign) {
    stream << (negative ? "-" : "+");
  }
  // print hex/octal prefix
  if (showbase) {
    if (hex) {
      stream << (upper ? "0X" : "0x");
    } else if (octal && value != 0) {
      stream << "0";  // 0 on front if value isn't already 0
    }
  }
  // pad internal (e.g. zero pad hex)
  if (internal) {
    for (; printed < width; printed++) {
      stream << (const char*)fillstr;
    }
  }
  // Print number
  stream << str;
  // pad remaining
  for (; printed < width; printed++) {
    stream << (const char*)fillstr;
  }
}

ostream& ostream::operator<<(uintmax_t value) {
  out(*this, value, flags_, width_, false, false, fill_);
  return *this;
}

ostream& ostream::operator<<(intmax_t value) {
  uintmax_t positive = static_cast<uintmax_t>(value);
  if (value < 0)
    positive = ~positive + 1;  // negate
  out(*this, positive, flags_, width_, true, value < 0, fill_);
  return *this;
}

ostream& ostream::operator<<(bool value) {
  return *this << ((flags() & ios::uppercase) != ios::fmtflags::none
                       ? value ? "TRUE" : "FALSE"
                   : value ? "true"
                           : "false");
}