#include "kernel.h"

using namespace k;

rtk::StatusCode kernel_main(const KernelContext& _) {
  auto status = rtk::StatusCode::Ok;

  // auto& allocator = k.pageAllocator();

  Context().memoryLayout().heapEnd();

  return status;
}