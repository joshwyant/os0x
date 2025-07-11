#include <stddef.h>

#include "kernel.h"

// <cstring>
namespace rtk {
extern "C" {
void* memcpy(void* dest, const void* src, size_t n) {
  auto d = static_cast<unsigned char*>(dest);
  auto s = static_cast<const unsigned char*>(src);
  while (n--) {
    *d++ = *s++;
  }
  return dest;
}

void memset(void* dest, unsigned char val, size_t n) {
  for (size_t i = 0; i < n; i++)
    ((unsigned char*)dest)[i] = val;
}
}  // extern "C"
}  // namespace rtk