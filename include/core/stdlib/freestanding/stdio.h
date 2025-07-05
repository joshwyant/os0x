#pragma once

#include <cstddef>

#ifdef __cplusplus
namespace rtk {
extern "C" {
#endif
void printf(const char* format, ...);
void sprintf(char* s, const char* format, ...);
void snprintf(char* s, size_t n, const char* format, ...);
#ifdef __cplusplus
}  // extern "C"
}  // namespace rtk
#endif  // cplusplus
