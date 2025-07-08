#include "kernel.h"

using namespace k;

rtk::StatusCode kernel_main(const KernelContext& k) {
  rtk::StatusCode status;

  auto& allocator = k.pageAllocator();

  Context().memoryLayout().heapEnd();

  return rtk::StatusCode::Ok;
}