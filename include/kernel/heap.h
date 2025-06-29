#pragma once

#include "di.h"
#include "kernel.h"

namespace k {
class SystemAllocator {
 public:
  Allocator(ServiceContext& sc) : sc_{sc} {}
  virtual ~Allocator() noexcept = 0;

 protected:
  ServiceContext& sc_;
};  // class heap
inline Allocator::~Allocator() noexcept {}
}  // namespace k