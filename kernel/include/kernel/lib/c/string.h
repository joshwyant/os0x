#pragma once

#include <stddef.h>

// Required since the c standard allows struct copies to use memcpy internally
void *memcpy(void *dest, const void *src, size_t n);
