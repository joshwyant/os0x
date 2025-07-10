#pragma once

#include "core/stdlib/string_view.h"
#include "core/stdlib/type_traits.h"
#include "core/stdlib/utility.h"

#define CHECK_STATUS()                   \
  do {                                   \
    if (status != rtk::StatusCode::Ok) { \
      return status;                     \
    }                                    \
  } while (0)

#define STATUS_LIST                                             \
  X(0x00000000, Ok, "Success")                                  \
  X(0x00000001, Unknown, "Unknown status")                      \
  X(0x00000002, Unspecified, "Unspecified error")               \
  X(0x00000003, OutOfBounds, "Access out of bounds")            \
  X(0x00000004, NotImplemented, "Function not implemented")     \
  X(0x00000005, OutOfMemory, "Out of memory")                   \
  X(0x00000006, OutOfRange, "Argument or value out of range")   \
  X(0x00000007, Uninitialized, "Using an uninitialized object") \
  X(0x00000008, InitializationError, "Failed to initialize")    \
  X(0x00000009, ValueNotPresent, "The requested value is not present")
#define STATUS_UNDEFINED "Undefined status code"

namespace rtk {
enum class [[nodiscard]] StatusCode {
#define X(val, name, msg) name = val,
  STATUS_LIST
#undef X
};  // enum class rtk::StatusCode

inline constexpr const char* StatusName(rtk::StatusCode code) {
  switch (code) {
#define X(val, name, msg)     \
  case rtk::StatusCode::name: \
    return #name;
    STATUS_LIST
#undef X
    default:
      return "";
  }
}

inline constexpr const char* StatusDescription(rtk::StatusCode code) {
  switch (code) {
#define X(val, name, msg)     \
  case rtk::StatusCode::name: \
    return msg;
    STATUS_LIST
#undef X
    default:
      return STATUS_UNDEFINED;
  }
}

inline constexpr const char* StatusString(rtk::StatusCode code) {
  switch (code) {
#define X(val, name, msg)     \
  case rtk::StatusCode::name: \
    return #name " (" #val "): " msg;
    STATUS_LIST
#undef X
    default:
      return STATUS_UNDEFINED;  // TODO: use a formatter to include the value for the code
  }
}

void StatusTrap(rtk::StatusCode code);
void StatusTrap(rtk::StatusCode code, string_view file, int line);
#define TRAP(code) StatusTrap(rtk::StatusCode::code, __FILE__, __LINE__);

#undef STATUS_LIST
#undef STATUS_UNDEFINED

template <typename T>
class StatusOr {
  bool hasT_;
  union {
    rtk::StatusCode status_;
    T item_;
  };

 public:
  using value_type = T;
  StatusOr(rtk::StatusCode status) : hasT_{false}, status_{status} {}
  StatusOr(T t) : hasT_{true}, item_{t} {}
  StatusOr() : StatusOr(rtk::StatusCode::Unknown) {}
  StatusOr(const StatusOr& other) : StatusOr() { *this = other; }
  StatusOr(StatusOr&& other) noexcept : StatusOr() { swap(*this, other); }
  StatusOr& operator=(const StatusOr& other) {
    if (this == &other)
      return *this;

    reset();
    hasT_ = other.hasT_;
    if (!hasT_) {
      status_ = other.status_;
    } else {
      item_ = other.item_;
    }

    return *this;
  }
  StatusOr& operator=(StatusOr&& other) noexcept {
    if (this == &other)
      return *this;

    reset();
    hasT_ = other.hasT_;
    if (!hasT_) {
      status_ = other.status_;
    } else {
      item_ = rtk::move(other.item_);
    }
    other.reset();

    return *this;
  }
  StatusOr& operator=(rtk::StatusCode status) {
    reset();
    status_ = status;
  }
  StatusOr& operator=(T&& item) {
    reset();
    hasT_ = true;
    item_ = rtk::move(item);
    return *this;
  }
  friend void swap(StatusOr& a, StatusOr& b) {
    // Use move assign manually to guarantee behavior
    StatusOr temp = move(a);
    a = move(b);
    b = move(temp);
  }
  constexpr operator rtk::StatusCode() const {
    if (hasT_) {
      return StatusCode::Ok;
    }
    return status_;
  }
  const T& get() const {
    if (!hasT_) {
      StatusTrap(status_);
    }
    return item_;
  }
  T& get() { return const_cast<T&>(as_const(*this).get()); }
  virtual ~StatusOr() { reset(); }
  bool operator!() { return !hasT_; }
  rtk::StatusCode status() const { return hasT_ ? StatusCode::Ok : status_; }
  bool ok() const { return hasT_; }

  template <typename F>
  auto map(F f) const -> StatusOr<decltype(f(item_))> {
    using U = decltype(f(item_));
    if (!ok()) {
      return StatusOr<U>(status_);
    }
    return StatusOr<U>(f(item_));
  }

  template <typename F>
  auto and_then(F&& f) const {
    using U = decltype(f(item_));
    // static_assert(rtk::is_same<U, StatusOr<typename U::value_type>>::value,
    //               "and_then must return StatusOr");

    if (!ok())
      return U(status_);
    return f(item_);
  }

 private:
  void reset() {
    if (hasT_) {
      item_.~T();
      hasT_ = false;
    }
    status_ = rtk::StatusCode::Unknown;
  }
};  // class StatusOr<T>

}  // namespace rtk