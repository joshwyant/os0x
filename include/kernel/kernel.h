#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/status.h"
#include "kernel/paging.h"

namespace k {
// forward declarations
class KernelContext;
class KernelBootstrapper;

// in init/init.cpp
const KernelContext& CreateContext(KernelBootstrapper& bootstrapper);
const KernelContext& Context();

class KernelContext {
 public:
  KernelContext(const KernelContext& other) = delete;
  KernelContext(KernelContext&& other) = delete;
  KernelContext& operator=(const KernelContext& other) = delete;
  KernelContext& operator=(KernelContext&& other) = delete;

  // paging.h
  const virtual KernelMemoryLayout& memoryLayout() const = 0;
  const virtual PhysicalMemoryAllocator& pageAllocator() const = 0;
  const virtual VirtualMemoryAllocator& virtualMemoryAllocator() const = 0;
  const virtual PageTables& pageTables() const = 0;

 protected:
  KernelContext() {};
  static KernelContext* context;

  friend const KernelContext& Context();
  friend const KernelContext& CreateContext(KernelBootstrapper& bootstrapper);
};  // class KernelContext

class DefaultKernelContext final : public KernelContext {
 public:
  // in kernel/init.cpp
  DefaultKernelContext(KernelBootstrapper& bootstrapper);

  const KernelMemoryLayout& memoryLayout() const override {
    return memoryLayout_;
  }
  const PhysicalMemoryAllocator& pageAllocator() const override {
    return *pageAllocatorPtr_;
  }
  const VirtualMemoryAllocator& virtualMemoryAllocator() const override {
    return virtualMemoryAllocator_;
  }
  const PageTables& pageTables() const override { return pageTables_; }

 protected:
  const DefaultKernelMemoryLayout memoryLayout_;
  const DefaultPhysicalMemoryAllocator pageAllocator_;
  const PhysicalMemoryAllocator* pageAllocatorPtr_;
  const DefaultVirtualMemoryAllocator virtualMemoryAllocator_;
  const RecursivePageTables pageTables_;
};  // class DefaultKernelContext

class KernelBootstrapper {
 public:
  KernelBootstrapper(const KernelBootstrapper& other) = delete;
  KernelBootstrapper(KernelBootstrapper&& other) = delete;
  KernelBootstrapper& operator=(const KernelBootstrapper& other) = delete;
  KernelBootstrapper& operator=(KernelBootstrapper&& other) = delete;

  virtual MemoryBootstrapper& memoryBootstrapper() = 0;

 protected:
  KernelBootstrapper() {}
};  // class KernelBootstrapper
}  // namespace k

// in main.cpp
rtk::StatusCode kernel_main(const k::KernelContext&);
