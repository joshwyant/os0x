#include "kernel.h"

#include "core/stdlib/freestanding/new.h"

using namespace k;

// Reserve a buffer for the kernel context
alignas(DefaultKernelContext) static uint8_t
    kernelBuf[sizeof(DefaultKernelContext)];

// Create a global singleton pointer (remove if inappropriate)
KernelContext* KernelContext::context = nullptr;

const KernelContext& k::CreateContext(KernelBootstrapper& bootstrapper) {
  // Create the kernel context, in place of the buffer, passing in parameters
  KernelContext::context = new (::kernelBuf) DefaultKernelContext{bootstrapper};

  return *KernelContext::context;
}

const KernelContext& k::Context() {
  return *KernelContext::context;
}

DefaultKernelContext::DefaultKernelContext(KernelBootstrapper& bootstrapper)
    : pageAllocator_{bootstrapper.memoryBootstrapper()},
      pageAllocatorPtr_(
          &bootstrapper.memoryBootstrapper().bootstrapAllocator()),
      virtualMemoryAllocator_{},
      pageTables_{memoryLayout_, pageAllocator_,  //virtualMemoryAllocator_,
                  bootstrapper.memoryBootstrapper()} {
  pageAllocator_.init(this, bootstrapper.memoryBootstrapper());
  pageAllocatorPtr_ = &pageAllocator_;
}