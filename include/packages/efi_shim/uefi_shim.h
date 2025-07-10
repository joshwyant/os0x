#pragma once

#include "core/stdlib/freestanding/new.h"
#include "kernel.h"

#include "packages/efi/bootinfo.h"

// forward declarations
class UefiMemoryBootstrapper;
class UefiKernelBootstrapper;
class UefiBootstrapPhysicalMemoryAllocator;

class UefiBootstrapPhysicalMemoryAllocator final
    : public k::PhysicalMemoryAllocator {
 public:
  UefiBootstrapPhysicalMemoryAllocator(UefiMemoryBootstrapper& parent)
      : parent_{parent} {};

  rtk::StatusOr<uintptr_t> allocatePage() const override;
  rtk::StatusOr<k::PageSet> allocatePages(size_t count) const override;
  size_t memorySize() const override;

 private:
  UefiMemoryBootstrapper& parent_;
};  // UefiBootstrapPhysicalMemoryAllocator

class UefiMemoryBootstrapper final : public k::MemoryBootstrapper {
 public:
  UefiMemoryBootstrapper(const UefiKernelBootstrapper& parent);

  ~UefiMemoryBootstrapper() noexcept override;
  const k::PhysicalMemoryAllocator& bootstrapAllocator() const override {
    return bootstrapAllocator_;
  }
  uintptr_t pageTablePhysicalAddress() const override {
    return pageTablePhysicalAddress_;
  }
  size_t memorySize() const override { return memSize_; }

  class UefiFreePhysicalMemoryRange final : public MemoryRangeSource {
   public:
    UefiFreePhysicalMemoryRange(UefiMemoryBootstrapper& parent)
        : parent_{parent} {
      parent_.descriptorIndex_ =
          0;  // not exactly a valid use of this iterator,
              // but should still technically allow
              // iteration to restart.
    }

    bool move_next() override;
    const k::PageSet& current() const override;

   private:
    UefiMemoryBootstrapper& parent_;
    k::PageSet current_;
  };

  MemoryRange processFreePhysicalMemoryPages() override;
  uintptr_t mappedVirtualMemoryEnd() const override {
    return nextFreeVirtualPage_;
  }
  rtk::StatusCode reserveVirtualMemory(size_t pageCount,
                                       uintptr_t* newAddr) override;
  const k::KernelMemoryLayout& layout() const override { return layout_; }

 private:
  friend class UefiFreePhysicalMemoryRange;
  friend class UefiBootstrapPhysicalMemoryAllocator;
  const k::DefaultKernelMemoryLayout layout_;
  const UefiBootstrapPhysicalMemoryAllocator bootstrapAllocator_;
  const uintptr_t pageTablePhysicalAddress_;
  const boot_memmap_t memoryMap_;
  const size_t descriptorCount_;
  int descriptorIndex_;
  UefiFreePhysicalMemoryRange physicalMemoryRange_;
  uintptr_t nextFreeVirtualPage_;
  size_t memSize_;
};  // class UefiMemoryBootstrapper

class UefiKernelBootstrapper final : public k::KernelBootstrapper {
 public:
  UefiKernelBootstrapper(const boot_info_t* bootInfo)
      : memoryBootstrapper_{*this}, bootInfo_(*bootInfo) {}

  k::MemoryBootstrapper& memoryBootstrapper() override {
    return memoryBootstrapper_;
  }
  const boot_info_t& bootInfo() const { return bootInfo_; }

 private:
  const boot_info_t bootInfo_;
  UefiMemoryBootstrapper memoryBootstrapper_;
};  // class UefiKernelBootstrapper

// A buffer for the uefi kernel bootstrapper
extern uint8_t bootstrapper_buf[];

static size_t calcMemSize(const boot_memmap_t& mmap);
static size_t descriptorCount(const boot_memmap_t& mm);
static EFI_MEMORY_DESCRIPTOR* descriptor(const boot_memmap_t& mm, size_t i);
static bool isFreeMem(UINT32 descriptorType);
static bool shouldBeUnmapped(UINT32 descriptorType);

inline size_t UefiBootstrapPhysicalMemoryAllocator::memorySize() const {
  return parent_.memorySize();
}

inline UefiMemoryBootstrapper::UefiMemoryBootstrapper(
    const UefiKernelBootstrapper& parent)
    : layout_{},
      bootstrapAllocator_{*this},
      physicalMemoryRange_{*this},
      pageTablePhysicalAddress_{parent.bootInfo().page_table_physical},
      nextFreeVirtualPage_{parent.bootInfo().memory_end},
      memoryMap_{parent.bootInfo().memory_map},
      memSize_{calcMemSize(parent.bootInfo().memory_map)},
      descriptorCount_{parent.bootInfo().memory_map.memory_map_size /
                       parent.bootInfo().memory_map.descriptor_size},
      descriptorIndex_{-1} {}