#pragma once

#include <stddef.h>

namespace rtk {
static constexpr size_t strlen(const char* str) {
  if (str == nullptr)
    return 0;
  auto len = 0;
  while (*(str++))
    len++;
  return len;
}  // strlen
static constexpr char* strcpy(char* dest, const char* src) {
  auto start = dest;
  if (dest != nullptr && dest != src) {
    if (src != nullptr) {
      while (*src)
        *(dest++) = *(src++);
      *dest = '\0';
    }
  }
  return start;
}  // strcpy
// https://cplusplus.com/reference/cstring/strncpy/
static constexpr char* strncpy(char* dest, const char* src, size_t n) {
  auto start = dest;
  size_t i = 0;
  if (dest != nullptr && dest != src) {
    if (src != nullptr) {
      while (*src && (i++ < n))
        *(dest++) = *(src++);
      // Fill all the rest with zeroes
      while ((i++ < n))
        *(dest++) = '\0';
    }
  }
  return start;
}  // strncpy
}  // namespace rtk