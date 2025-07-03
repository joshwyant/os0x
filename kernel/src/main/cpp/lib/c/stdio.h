#pragma once

#include <cstddef>
#include "core/stdlib/freestanding/stdio.h"

enum class FormatFlags {
  None,
  LeftJustify,
  PlusPrefix,
  Space,
  Hash,
  Zero,
};
static FormatFlags& operator|=(FormatFlags& a, FormatFlags b) {
  a = static_cast<FormatFlags>(static_cast<int>(a) | static_cast<int>(b));
  return a;
}

class Formatter {
 public:
  Formatter() : charsPrinted_{0} {}

  void parsef(const char* format, ...) const;
  int nparsef(const char* format, size_t count, ...) const;

 protected:
  virtual size_t outputChars(const char* chars, size_t length) const = 0;
  virtual size_t outputChars(const char* chars) const = 0;
  mutable size_t charsPrinted_;

 private:
  mutable const char* format_;
  mutable const char* c_;
  mutable FormatFlags flags_;
  mutable size_t size_;
  mutable int width_;
  mutable int precision_;
  void parseFormatSpecifier() const;
  void parseFlags() const;
  int parseWidth() const;
  void parsePrecision() const;
  void parseLength() const;
  uintmax_t widenArg(size_t size, bool isSigned, ...) const;
  void outputInteger(uintmax_t val, bool isSigned) const;
  void outputHex(uintmax_t val, bool upper) const;
  void outputOctal(uintmax_t val) const { /* not supported */ }
};

class StringFormatter : public Formatter {
 public:
  StringFormatter(char* output) {}

 protected:
  size_t outputChars(const char* chars, size_t length) const override;
};