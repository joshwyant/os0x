#include "core/stdlib/freestanding/stdint.h"
#include "core/stdlib/freestanding/string.h"
#include "kernel.h"

using namespace k;

constexpr auto kBitsPerByte = UINT8_WIDTH;
constexpr auto kBitsPerPage = kPageSize * kBitsPerByte;
constexpr auto kByteMaskNoBitsSet = (uint8_t)0x00U;
constexpr auto kByteMaskAllBitsSet = (uint8_t)0xFFU;
constexpr auto kLongMaskAllBitsSet = 0xFFFFFFFFFFFFFFFFULL;
constexpr auto kByteMaskRightmostBitSet = (uint8_t)1;

uint8_t makeSetBitsMask(size_t& currentPage, size_t endPage);
uint8_t makeUnsetBitsMask(size_t& currentPage, size_t endPage);

#define CHECK_INIT_STATUS()                           \
  do {                                                \
    if (initializationStatus_ != rtk::StatusCode::Ok) \
      return rtk::StatusCode::InitializationError;    \
  } while (0)

RecursivePageTables::RecursivePageTables(
    const KernelMemoryLayout& layout, const PhysicalMemoryAllocator& pallocator,
    // const VirtualMemoryAllocator& vallocator,
    const MemoryBootstrapper& memoryBootstrapper)

    : PageTables(memoryBootstrapper.pageTablePhysicalAddress(),
                 layout.pageTableAddress()),
      tablesStart_{layout.pageTablesStart()},
      // tablesEnd_{layout.pageTablesEnd()},
      pallocator_{pallocator} /*,
      vallocator_{vallocator}*/
{}

rtk::StatusCode RecursivePageTables::map(uintptr_t virtAddr, uintptr_t physAddr,
                                         PageAttr attributes) const {
  volatile PageTable* table = &pml4_;

  // Recurse page tables and make sure they exist; find the final page table
  for (auto level = PageLevel::PML4; level > PageLevel::PT; level--) {
    // Get the next entry from the table using the virtual address
    auto idx = entryIndex(virtAddr, level);
    auto entryResult = table->getEntry(idx);
    if (!entryResult.ok())
      return entryResult.status();  // out of bounds
    auto entry = entryResult.get();

    // Get the subtable by calculating its address, potentially creating it if entry doesn't exist
    // Parameterized subtable addressing is a property of recursive page tables.
    volatile auto subtable =
        (volatile PageTable*)tableAddress(virtAddr, level - 1);
    if (!entry->present()) {
      // Allocate a page for the subtable
      auto allocateResult = pallocator_.allocatePage();
      if (!allocateResult)
        return allocateResult.status();  // out of mem?
      auto subtableNewPaddr = allocateResult.get();

      // Bring subtable into virtual page table mapping space (calls invlpg)
      // This is again possible thanks to recursive mapping.
      mapNoTables((uintptr_t)subtable, subtableNewPaddr,
                  PageAttr::Present | PageAttr::RW);

      // Clear the new page table
      subtable->clear();
    }

    // Move down to the next table
    // This will have just been mapped in
    table = subtable;
  }

  // Set the new entry in the lowest page table (calls invlpg)
  mapNoTables(virtAddr, physAddr, attributes);

  return rtk::StatusCode::Ok;
}

uint8_t makeSetBitsMask(size_t& currentPage, size_t endPage) {
  auto byteMask = kByteMaskNoBitsSet;
  if (currentPage >= endPage)
    return 0;
  do {  // allow first bit to be byte-aligned
    auto currentBit = currentPage % UINT8_WIDTH;
    // clear the bit in the mask
    byteMask |= static_cast<uint8_t>(kByteMaskRightmostBitSet << currentBit);
    currentPage++;
  } while (currentPage % UINT8_WIDTH != 0 && currentPage < endPage);
  return byteMask;
}

uint8_t makeUnsetBitsMask(size_t& currentPage, size_t endPage) {
  return static_cast<uint8_t>(~makeSetBitsMask(currentPage, endPage));
}

void DefaultPhysicalMemoryAllocator::init(
    const KernelContext* kernel, MemoryBootstrapper& bootstrapper) const {
  auto& status = initializationStatus_;  // alias

  // Get the page frame bitmap parameters right
  const auto totalMemPages = bootstrapper.memorySize() / kPageSize;
  auto pagesNeeded =
      (totalMemPages + kBitsPerPage - 1) / kBitsPerPage;  // round up
  bitmapSize_ = pagesNeeded * kPageSize;

  // Reserve our virtual memory space
  auto memResult = bootstrapper.reserveVirtualMemory(pagesNeeded);
  if (!memResult)  // out of mem?
    return;

  const auto bitmapStart = memResult.get();
  bitmap_ = reinterpret_cast<uint8_t*>(bitmapStart);

  auto currentPage = bitmapStart;

  // Keep allocating [contiguous?] physical pages (bootstrap allocator used behind the scenes)
  // until we have enough for the bitmap; Allocate and map them to virtual memory along the way
  while (pagesNeeded > 0) {
    // Allocate physical pages
    auto memResult = kernel->pageAllocator().allocatePages(pagesNeeded);
    if (!memResult.ok())
      return;  // out of physical mem?
    auto newPages = memResult.get();

    const auto bytesAllocated = newPages.count * kPageSize;

    // Map them to virtual memory
    status =
        kernel->pageTables().map(newPages.count, currentPage, newPages.address,
                                 PageAttr::Present | PageAttr::RW);
    if (status != rtk::StatusCode::Ok)  // out of mem creating tables?
      return;

    // Clear the page (all 1's for "used" by default)
    rtk::memset(reinterpret_cast<void*>(currentPage), kByteMaskAllBitsSet,
                kPageSize);

    // Advance
    currentPage += bytesAllocated;
    pagesNeeded -= newPages.count;
  }

  // Enumerate physical memory and mark where it's free
  for (auto range : bootstrapper.processFreePhysicalMemoryPages()) {
    const auto startPage = range.address / kPageSize;
    auto endPage = startPage + range.count;
    auto currentPage = startPage;

    // We can initialize the lowest free page number
    if (startPage < lowestFreePage_) {
      lowestFreePage_ = startPage;
    }

    // Build a bitmap for the first byte
    if (currentPage % UINT8_WIDTH != 0) {
      auto byteMask = makeUnsetBitsMask(currentPage, endPage);
      bitmap_[startPage / UINT8_WIDTH] &= byteMask;
    }

    // Write 0's for whole bytes
    for (; currentPage + UINT8_WIDTH <= endPage; currentPage += UINT8_WIDTH) {
      bitmap_[currentPage / UINT8_WIDTH] = kByteMaskNoBitsSet;
    }

    // final byte
    if (currentPage < endPage) {
      auto lastByteIndex = currentPage / UINT8_WIDTH;
      auto byteMask = makeUnsetBitsMask(currentPage, endPage);
      bitmap_[lastByteIndex] &= byteMask;
    }
  }
  initializationStatus_ = rtk::StatusCode::Ok;
}

DefaultPhysicalMemoryAllocator::DefaultPhysicalMemoryAllocator(
    MemoryBootstrapper& memoryBootstrapper)
    : memorySize_(memoryBootstrapper.memorySize()),
      lowestFreePage_(UINTPTR_MAX) {}

rtk::StatusOr<uintptr_t> DefaultPhysicalMemoryAllocator::allocatePage() const {
  return allocatePages(1).map([](auto pageSet) { return pageSet.address; });
}

template <typename T>
bool DefaultPhysicalMemoryAllocator::alignedFreeCheckAdvanceAndMark(
    size_t& pageNo, size_t count, size_t* pagesAllocated) const {
  constexpr auto sizeBits = sizeof(T) * kBitsPerByte;
  const auto pageMapBits = bitmapSize_ * UINT8_WIDTH;

  // Bounds check
  if (pageNo + sizeBits > pageMapBits)
    return false;

  // We can't use this if we're checking too many bits
  if (count - *pagesAllocated < sizeBits)
    return false;

  const auto ptr = reinterpret_cast<T*>(bitmap_);
  const auto i = pageNo / sizeBits;

  const auto isFree = ptr[i] == 0;
  if (isFree) {
    // Mark chunk as used
    ptr[i] = static_cast<T>(kLongMaskAllBitsSet);

    // Advance
    pageNo += sizeBits - 1;  // Account for page++ in outside loop
    *pagesAllocated += sizeBits;
  }

  return isFree;
}

rtk::StatusOr<PageSet> DefaultPhysicalMemoryAllocator::allocatePages(
    size_t count) const {
  CHECK_INIT_STATUS();

  if (count == 0) {
    return rtk::StatusCode::OutOfRange;
  }

  const auto startPage = lowestFreePage_;
  const auto bitmapPages = bitmapSize_ * UINT8_WIDTH;
  size_t pagesAllocated = 0;

  // Enumerate bitmap in chunks of 1 or 8*2^x bits
  for (auto page = startPage; page < bitmapPages; page++) {
    // const auto bitmapPagesLeft = bitmapPages - page;
    const auto pagesNeeded = count - pagesAllocated;
    const auto byte = page / UINT8_WIDTH;
    const auto bit = page % UINT8_WIDTH;
    const auto align = bit % UINT64_WIDTH;

    // Fast way--try a chunk first.
    switch (align) {
      // 64-bit boundary
      case 0:
        if (alignedFreeCheckAdvanceAndMark<uint64_t>(page, count,
                                                     &pagesAllocated))
          continue;
        break;
      // 32-bit boundary
      case UINT32_WIDTH:
        if (alignedFreeCheckAdvanceAndMark<uint32_t>(page, count,
                                                     &pagesAllocated))
          continue;
        break;
      // 16-bit boundary
      case UINT16_WIDTH:
      case UINT16_WIDTH * 3:
        if (alignedFreeCheckAdvanceAndMark<uint16_t>(page, count,
                                                     &pagesAllocated))
          continue;
        break;
      // 8-bit boundary
      case UINT8_WIDTH:
      case UINT8_WIDTH * 3:
      case UINT8_WIDTH * 5:
      case UINT8_WIDTH * 7:
        if (alignedFreeCheckAdvanceAndMark<uint8_t>(page, count,
                                                    &pagesAllocated))
          continue;
        break;
    }

    // Slow way, use partial byte
    auto mask = kByteMaskRightmostBitSet << bit;
    auto isFree = (bitmap_[byte] & mask) == kByteMaskNoBitsSet;

    if (!isFree) {
      if (pagesAllocated == 0)
        continue;  // Try another page;
      break;  // Or, we have broken continuity, that's all the pages we can allocate at this address.
    }

    // Curteously find and update lowestFreePage once we have enough pages
    if (pagesNeeded == 0) {
      lowestFreePage_ = page;
      break;
    }

    // Mark used
    bitmap_[byte] |= mask;
    pagesAllocated++;
  }

  if (pagesAllocated == 0) {
    lowestFreePage_ = bitmapPages;  // literally no lowest free page
    return rtk::StatusCode::OutOfMemory;
  }

  uintptr_t newPhysicalAddress = startPage * kPageSize;
  return PageSet{kPageSize, newPhysicalAddress, pagesAllocated};
}

rtk::StatusOr<uintptr_t> DefaultVirtualMemoryAllocator::allocatePage() const {
  return rtk::StatusCode::NotImplemented;
}

rtk::StatusOr<PageSet> DefaultVirtualMemoryAllocator::allocatePages(
    size_t _) const {
  return rtk::StatusCode::NotImplemented;
}