#include <stddef.h>

#include "kernel.h"

// <string.h>
void* memcpy(void* dest, const void* src, size_t n) {
  unsigned char* d = dest;
  const unsigned char* s = src;
  while (n--) {
    *d++ = *s++;
  }
  return dest;
}

void memset(void* dest, unsigned char val, size_t n) {
  for (size_t i = 0; i < n; i++)
    ((unsigned char*)dest)[i] = val;
}
