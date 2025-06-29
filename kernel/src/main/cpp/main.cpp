#include "kernel.h"

using namespace k;

StatusCode kernel_main(const KernelContext& k) {
  StatusCode status;

  auto& allocator = k.pageAllocator();

  Context().memoryLayout().heapEnd();

  return StatusCode::Ok;
}