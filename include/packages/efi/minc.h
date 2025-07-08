#pragma once

// Minimal replacement definitions for some missing C library functions

#include <stddef.h>
#include <stdint.h>

#define strtoul(a, b, c) _strtoul(a)
#define strncmp(a, b, c) _strncmp(a, b, c)
#define strlen(a) _strlen(a)
#define memcpy(a, b, c) _memcpy(a, b, c)
#define memset(a, b, c) _memset(a, b, c)
#define memcmp(a, b, c) _memcmp(a, b, c)

static uint32_t _strtoul(const char* p) {
  uint32_t n = 0;
  int c = 8;  // uint32 hex chars count
  while (*p != 0 && c--) {
    char c = *p | 0x20;  // Force lowercase
    int i;
    if (c >= '0' && c <= '9') {
      i = c - '0';
    } else if (c >= 'a' && c <= 'f') {
      i = c - 'a' + 10;
    } else
      return 0;
    n = n * 16 + i;
    p++;
  }
  return n;
}

static int _strncmp(const char* a, const char* b, size_t n) {
  while (n-- && (*a || *b)) {
    if (*a != *b)
      return *(unsigned char*)a - *(unsigned char*)b;
    a++;
    b++;
  }
  return 0;
}

static size_t _strlen(const char* s) {
  size_t len = 0;
  while (*s++)
    len++;
  return len;
}

static void* _memcpy(void* dest, const void* src, size_t n) {
  unsigned char* d = (unsigned char*)dest;
  const unsigned char* s = (const unsigned char*)src;
  while (n--)
    *d++ = *s++;
  return dest;
}

static void _memset(void* dest, unsigned char val, size_t n) {
  for (size_t i = 0; i < n; i++)
    ((unsigned char*)dest)[i] = val;
}

static int _memcmp(const void* a, const void* b, size_t n) {
  const unsigned char *pa = (const unsigned char*)a,
                      *pb = (const unsigned char*)b;
  for (size_t i = 0; i < n; i++) {
    if (pa[i] != pb[i])
      return pa[i] - pb[i];
  }
  return 0;
}