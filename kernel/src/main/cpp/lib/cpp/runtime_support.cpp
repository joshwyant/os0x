#include "core/stdlib/freestanding/new.h"

// <new>
void* operator new(size_t, void* ptr) noexcept {
  return ptr;
}
void operator delete(void*, void*) noexcept {}  // no-op
void operator delete(void*) noexcept {}
void operator delete(void*, size_t) noexcept {}

extern "C" void
__cxa_deleted_virtual() { /* Fail: can't call deleted virtual method */ }
extern "C" void
__cxa_pure_virtual() { /* Fail: can't call pure virtual method */ }