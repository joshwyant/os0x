#include <efi.h>
#include "kernel.h"

#include "packages/efi_shim/uefi_shim.h"

using namespace k;

// called from arch/*/start.S
extern "C" void kernel_boot_uefi(const boot_info_t* bootInfo) {
  const KernelContext* kernelContext;

  // Sanity check: basic boot info validation
  if (bootInfo == NULL || bootInfo->magic != boot_info_t::BOOTINFO_MAGIC)
    freeze();

  constexpr auto kCatpuccinMochaMantleColor = 0x00181825;
  // clear_screen(bootInfo,
  //              kCatpuccinMochaMantleColor);  // catppuccin mocha mantle

  // Create a bootstrapper used to initialize the kernel with UEFI-specific data
  // Let it go out of scope for final bootstrapper cleanup of memory
  {
    auto bootstrapper = UefiKernelBootstrapper{bootInfo};

    // Create the kernel context which provides all the classes
    // for dependency injection
    kernelContext = &CreateContext(bootstrapper);
  }

  // Call kernel_main
  rtk::StatusCode status = kernel_main(*kernelContext);

  // Loop forever
  freeze();
}

UefiMemoryBootstrapper::~UefiMemoryBootstrapper() noexcept {
  // Unmap UEFI loader code and data that was identity-mapped in at boot/efi/virtual.cpp, map_virtual_address_space()
  for (auto i = 0; i < descriptorCount(memoryMap_); i++) {
    auto& d = *descriptor(memoryMap_, i);

    if (shouldBeUnmapped(d.Type)) {
      for (auto j = 0; j < d.NumberOfPages; j++) {
        auto entry = PT_ENTRY_PTR(d.PhysicalStart + EFI_PAGE_SIZE * j);
        *entry = 0;  // Unmap the page.
      }
      break;
    }
  }
}

MemoryBootstrapper::MemoryRange
UefiMemoryBootstrapper::processFreePhysicalMemoryPages() {
  return MemoryRange{physicalMemoryRange_};
}

rtk::StatusCode UefiMemoryBootstrapper::reserveVirtualMemory(
    size_t pageCount, uintptr_t* newAddr) {
  if (nextFreeVirtualPage_ + pageCount * kPageSize > layout_.memoryMapsEnd()) {
    return rtk::StatusCode::OutOfMemory;
  }

  *newAddr = nextFreeVirtualPage_;
  nextFreeVirtualPage_ += pageCount * EFI_PAGE_SIZE;

  return rtk::StatusCode::Ok;
}

rtk::StatusCode UefiBootstrapPhysicalMemoryAllocator::allocatePage(
    uintptr_t* newPhysicalAddressOut) const {
  size_t pagesAllocated;  // discard
  return allocatePages(1, newPhysicalAddressOut, &pagesAllocated);
}

rtk::StatusCode UefiBootstrapPhysicalMemoryAllocator::allocatePages(
    size_t count, uintptr_t* newPhysicalAddressOut,
    size_t* pagesAllocated) const {
  *newPhysicalAddressOut = 0;
  *pagesAllocated = 0;

  // Start past current; on something that hasn't already been reported as "free"
  for (auto i = parent_.descriptorIndex_ + 1; i < parent_.descriptorCount_;
       i++) {
    auto& d = *descriptor(parent_.memoryMap_, i);
    if (d.NumberOfPages > 0 && isFreeMem(d.Type)) {
      *newPhysicalAddressOut = d.PhysicalStart;
      *pagesAllocated = count < d.NumberOfPages ? count : d.NumberOfPages;
      d.NumberOfPages -= *pagesAllocated;
      d.PhysicalStart += *pagesAllocated * EFI_PAGE_SIZE;
      return rtk::StatusCode::Ok;
    }
  }

  return rtk::StatusCode::OutOfMemory;
}

bool UefiMemoryBootstrapper::UefiFreePhysicalMemoryRange::move_next() {
  for (auto& i = parent_.descriptorIndex_; i < parent_.descriptorCount_; i++) {
    auto& d = *descriptor(parent_.memoryMap_, i);
    if (d.NumberOfPages > 0 && isFreeMem(d.Type)) {
      current_ = {EFI_PAGE_SIZE, d.PhysicalStart, d.NumberOfPages};
      d.NumberOfPages = 0;  // Prevent re-use if iterated over again
      return true;
    }
  }

  return false;
}

const MemoryBootstrapper::PageSet&
UefiMemoryBootstrapper::UefiFreePhysicalMemoryRange::current() const {
  return current_;
}

inline size_t descriptorCount(const boot_memmap_t& mm) {
  return mm.memory_map_size / mm.descriptor_size;
}

inline EFI_MEMORY_DESCRIPTOR* descriptor(const boot_memmap_t& mm, size_t i) {
  if (i < 0 || i >= descriptorCount(mm)) {
    return nullptr;
  }

  auto descriptorAddr =
      reinterpret_cast<uintptr_t>(mm.memory_map) + i * mm.descriptor_size;
  return reinterpret_cast<EFI_MEMORY_DESCRIPTOR*>(descriptorAddr);
}
size_t calcMemSize(const boot_memmap_t& mm) {
  size_t extent = 0;
  size_t count = descriptorCount(mm);
  for (auto i = 0; i < count; i++) {
    auto& d = *descriptor(mm, i);
    if (isFreeMem(d.Type)) {
      auto end = d.PhysicalStart + EFI_PAGE_SIZE * d.NumberOfPages;
      if (end > extent) {
        extent = end;
      }
    }
  }

  return extent;
}

bool shouldBeUnmapped(UINT32 descriptorType) {
  switch (descriptorType) {
    case EfiLoaderCode:
    case EfiLoaderData:
      // TODO: consider more types
      return true;
    // Other types kept for reference
    case EfiConventionalMemory:
    case EfiReservedMemoryType:
    case EfiBootServicesCode:
    case EfiBootServicesData:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
    case EfiUnusableMemory:
    case EfiACPIReclaimMemory:
    case EfiACPIMemoryNVS:
    case EfiMemoryMappedIO:
    case EfiMemoryMappedIOPortSpace:
    case EfiPalCode:
    case EfiPersistentMemory:
    case EfiUnacceptedMemoryType:
    case EfiMaxMemoryType:
    default:
      return false;
  }
}

bool isFreeMem(UINT32 descriptorType) {
  switch (descriptorType) {
    case EfiConventionalMemory:
      // TODO: consider more types
      return true;
    case EfiReservedMemoryType:
    case EfiLoaderCode:
    case EfiLoaderData:
    case EfiBootServicesCode:
    case EfiBootServicesData:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
    case EfiUnusableMemory:
    case EfiACPIReclaimMemory:
    case EfiACPIMemoryNVS:
    case EfiMemoryMappedIO:
    case EfiMemoryMappedIOPortSpace:
    case EfiPalCode:
    case EfiPersistentMemory:
    case EfiUnacceptedMemoryType:
    case EfiMaxMemoryType:
    default:
      return false;
  }
}
