#pragma once

#include <cstddef>

// Implemented in kernel/lib/cpp/runtime_support.cpp
void* operator new(size_t, void* ptr) noexcept;
void operator delete(void*, void*) noexcept;
void operator delete(void*) noexcept;
void operator delete(void*, size_t) noexcept;