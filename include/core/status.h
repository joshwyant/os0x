#pragma once

#define CHECK_STATUS()              \
  do {                              \
    if (status != StatusCode::Ok) { \
      return status;                \
    }                               \
  } while (0)

#define STATUS_LIST                                             \
  X(0x00000000, Ok, "Success")                                  \
  X(0x00000001, Unspecified, "Unspecified error")               \
  X(0x00000002, OutOfBounds, "Access out of bounds")            \
  X(0x00000003, NotImplemented, "Function not implemented")     \
  X(0x00000004, OutOfMemory, "Out of memory")                   \
  X(0x00000005, OutOfRange, "Argument or value out of range")   \
  X(0x00000006, Uninitialized, "Using an uninitialized object") \
  X(0x00000007, InitializationError, "Failed to initialize")
#define STATUS_UNDEFINED "Undefined status code"

namespace rtk {
enum class [[nodiscard]] StatusCode {
#define X(val, name, msg) name = val,
  STATUS_LIST
#undef X
};  // enum class StatusCode

inline const char* StatusName(StatusCode code) {
  switch (code) {
#define X(val, name, msg) \
  case StatusCode::name:  \
    return #name;
    STATUS_LIST
#undef X
    default:
      return "";
  }
}

inline const char* StatusDescription(StatusCode code) {
  switch (code) {
#define X(val, name, msg) \
  case StatusCode::name:  \
    return msg;
    STATUS_LIST
#undef X
    default:
      return STATUS_UNDEFINED;
  }
}

inline const char* StatusString(StatusCode code) {
  switch (code) {
#define X(val, name, msg) \
  case StatusCode::name:  \
    return #name " (" #val "): " msg;
    STATUS_LIST
#undef X
    default:
      return STATUS_UNDEFINED;  // TODO: use a formatter to include the value for the code
  }
}

#undef STATUS_LIST
#undef STATUS_UNDEFINED

}  // namespace rtk