#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
// Implemented in kernel/lib/c/runtime_support.c
// Required since the c standard allows struct copies to use memcpy internally
void* memcpy(void* dest, const void* src, size_t n);
void memset(void* dest, unsigned char val, size_t n);
#ifdef __cplusplus
}  // cplusplus
#endif