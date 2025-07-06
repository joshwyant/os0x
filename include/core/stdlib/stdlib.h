#pragma once

#ifdef __cplusplus
namespace rtk {
extern "C" {
#endif
void panic(const char* msg);
#ifdef __cplusplus
}  // extern "C"
}  // namespace rtk
#endif  // cplusplus