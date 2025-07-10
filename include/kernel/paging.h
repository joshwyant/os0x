#pragma once
#include <stddef.h>
#include <stdint.h>
#include "asm.h"
#include "core/status.h"
#include "core/stdlib/freestanding/string.h"
#include "core/stdlib/utility.h"

namespace k {
class KernelMemoryLayout;
enum class PageAttr : uintptr_t;
class PageEntry;
class PageTable;
class PhysicalMemoryAllocator;
class VirtualMemoryAllocator;
class MemoryBootstrapper;
class KernelBootstrapper;

class KernelContext;

constexpr size_t kPageSize = 0x1000;  // 4096 bytes

struct PageSet {
  size_t pageSize;
  uintptr_t address;
  size_t count;
};

class KernelMemoryLayout {
 public:
  KernelMemoryLayout(uintptr_t userStart, uintptr_t userEnd,
                     uintptr_t kernelStart, uintptr_t kernelEnd,
                     uintptr_t identityStart, uintptr_t identityEnd,
                     uintptr_t heapStart, uintptr_t heapEnd,
                     uintptr_t kernelImageStart, uintptr_t mmapsStart,
                     uintptr_t mmapsEnd, uintptr_t pgtablesStart,
                     uintptr_t pgtablesEnd, uintptr_t pgtableAddr)
      : userStart_{userStart},
        userEnd_{userEnd},
        kernelStart_{kernelStart},
        kernelEnd_{kernelEnd},
        identityStart_{identityStart},
        identityEnd_{identityEnd},
        heapStart_{heapStart},
        heapEnd_{heapEnd},
        kernelImageStart_{kernelImageStart},
        mmapsStart_{mmapsStart},
        mmapsEnd_{mmapsEnd},
        pgtablesStart_{pgtablesStart},
        pgtablesEnd_{pgtablesEnd},
        pgtableAddr_{pgtableAddr} {}
  uintptr_t userSpaceStart() const { return userStart_; }
  uintptr_t userSpaceEnd() const { return userEnd_; }
  uintptr_t kernelSpaceStart() const { return kernelStart_; }
  uintptr_t kernelSpaceEnd() const { return kernelEnd_; }
  uintptr_t identityPagingStart() const { return identityStart_; }
  uintptr_t identityPagingEnd() const { return identityEnd_; }
  uintptr_t heapStart() const { return heapStart_; }
  uintptr_t heapEnd() const { return heapEnd_; }
  uintptr_t kernelImageStart() const { return kernelImageStart_; }
  uintptr_t memoryMapsStart() const { return mmapsStart_; }
  uintptr_t memoryMapsEnd() const { return mmapsEnd_; }
  uintptr_t pageTablesStart() const { return pgtablesStart_; }
  uintptr_t pageTablesEnd() const { return pgtablesEnd_; }
  uintptr_t pageTableAddress() const { return pgtableAddr_; }

 private:
  const uintptr_t userStart_;
  const uintptr_t userEnd_;
  const uintptr_t kernelStart_;
  const uintptr_t kernelEnd_;
  const uintptr_t identityStart_;
  const uintptr_t identityEnd_;
  const uintptr_t heapStart_;
  const uintptr_t heapEnd_;
  const uintptr_t kernelImageStart_;
  const uintptr_t mmapsStart_;
  const uintptr_t mmapsEnd_;
  const uintptr_t pgtablesStart_;
  const uintptr_t pgtablesEnd_;
  const uintptr_t pgtableAddr_;
};  // class KernelMemoryLayout

class DefaultKernelMemoryLayout : public KernelMemoryLayout {
 public:
  DefaultKernelMemoryLayout()
      : KernelMemoryLayout(
            0x0000000000000000ULL,  // user space start:         0
            0x00007FFFFFFFFFFFULL,  // user space end:         128 TiB
            0xFFFF800000000000ULL,  // kernel space start:    -128 TiB
            0xFFFFFFFFFFFFFFFFULL,  // kernel space end:      -
            0xFFFF800000000000ULL,  // identity maps start:   -128 TiB
            0xFFFF800FFFFFFFFFULL,  // identity maps end:     -127 TiB
            0xFFFF801000000000ULL,  // heap start             -127 TiB
            0xFFFFFEFFFFFFFFFFULL,  // heap end               -  1 TiB
            0xFFFFFF0000000000ULL,  // kernel image start     -  1 TiB
            0xFFFF801000000000ULL,  // memory maps start      -  1 TiB
            0xFFFFFF7FFFFFFFFFULL,  // memory maps end        -512 GiB
            0xFFFFFF8000000000ULL,  // page tables start      -512 GiB
            0xFFFFFFFFFFFFFFFFULL,  // page tables end        -
            0xFFFFFFFFFFFFF000ULL)  // page table PML4        -  4 KiB
  {}
};  // class DefaultKernelMemoryLayout

enum class PageAttr : uintptr_t {
  Present = (1ULL << 0),
  RW = (1ULL << 1),
  User = (1ULL << 2),
  PWT = (1ULL << 3),
  PCD = (1ULL << 4),
  Accessed = (1ULL << 5),
  Dirty = (1ULL << 6),
  PAT = (1ULL << 7),
  Global = (1ULL << 8),
  NX = (1ULL << 63),  // Only if EFER.NXE is enabled
};  // enum class PageAttr

inline PageAttr operator|(PageAttr a, PageAttr b) {
  return static_cast<PageAttr>(static_cast<uintptr_t>(a) |
                               static_cast<uintptr_t>(b));
}

inline PageAttr operator&(PageAttr a, PageAttr b) {
  return static_cast<PageAttr>(static_cast<uintptr_t>(a) &
                               static_cast<uintptr_t>(b));
}

inline PageAttr& operator|=(PageAttr& a, PageAttr b) {
  return a = a | b;
}

inline PageAttr& operator&=(PageAttr& a, PageAttr b) {
  return a = a & b;
}

inline PageAttr& operator~(PageAttr& a) {
  return a = static_cast<PageAttr>(~static_cast<uintptr_t>(a));
}

class __attribute__((aligned(8))) __attribute__((packed)) PageEntry {
 public:
  PageEntry() { value_ = 0; }
  PageEntry(uintptr_t entry) { value_ = entry; }
  PageEntry(PageEntry& other) { *this = other; }
  PageEntry(PageEntry&& other) { swap(*this, other); }
  PageEntry(uintptr_t pageBase, PageAttr attributes) {
    value_ =
        (pageBase & 0x000FFFFFFFFFF000ULL) | static_cast<uintptr_t>(attributes);
  }
  PageEntry(void* pageBase, PageAttr attributes)
      : PageEntry(reinterpret_cast<uintptr_t>(pageBase) |
                  static_cast<uintptr_t>(attributes)) {}
  volatile PageEntry& operator=(const volatile PageEntry& other) volatile {
    if (this != &other) {
      value_ = other.value_;
    }
    return *this;
  }
  volatile PageEntry& operator=(volatile PageEntry&& other) volatile {
    if (this != &other) {
      swap(*this, other);
    }
    return *this;
  }
  PageAttr attributes() const volatile {
    return static_cast<PageAttr>(value_ & 0xFFF0000000000FFFULL);
  }
  void clear() volatile { value_ = 0; }
  void setAttributes(PageAttr newAttributes) volatile {
    value_ =
        value_ & 0x000FFFFFFFFFF000ULL | static_cast<uintptr_t>(newAttributes);
  }
  void addAttributes(PageAttr newAttributes) volatile {
    value_ |= static_cast<uintptr_t>(newAttributes);
  }
  void intersectAttributes(PageAttr otherAttributes) volatile {
    setAttributes(attributes() & otherAttributes);
  }
  void removeAttributes(PageAttr oldAttributes) volatile {
    setAttributes(attributes() & ~oldAttributes);
  }
  uintptr_t physicalAddress() const volatile { return addressPart_ << 12; }
  void setPhysicalAddress(uintptr_t address) volatile {
    addressPart_ = address >> 12;
  }
  void setPhysicalAddress(const void* pointer) volatile {
    addressPart_ = reinterpret_cast<uintptr_t>(pointer) >> 12;
  }
  bool present() const volatile { return present_; }
  void setPresent(bool set) volatile { present_ = set; }
  void setPresent() volatile { setPresent(true); }
  void clearPresent() volatile { setPresent(false); }
  bool rw() const volatile { return rw_; }
  void setRw(bool set) volatile { rw_ = set; }
  void setRw() volatile { setRw(true); }
  void clearRw() volatile { setRw(false); }
  bool user() const volatile { return user_; }
  void setUser(bool set) volatile { user_ = set; }
  void setUser() volatile { setUser(true); }
  void clearUser() volatile { setUser(false); }
  bool pwt() const volatile { return pwt_; }
  void setPwt(bool set) volatile { pwt_ = set; }
  void setPwt() volatile { setPwt(true); }
  void clearPwt() volatile { setPwt(false); }
  bool pcd() const volatile { return pcd_; }
  void setPcd(bool set) volatile { pcd_ = set; }
  void setPcd() volatile { setPcd(true); }
  void clearPcd() volatile { setPcd(false); }
  bool accessed() const volatile { return accessed_; }
  void setAccessed(bool set) volatile { accessed_ = set; }
  void setAccessed() volatile { setAccessed(true); }
  void clearAccessed() volatile { setAccessed(false); }
  bool dirty() const volatile { return dirty_; }
  void setDirty(bool set) volatile { dirty_ = set; }
  void setDirty() volatile { setDirty(true); }
  void clearDirty() volatile { setDirty(false); }
  bool pat() const volatile { return pat_; }
  void setPat(bool set) volatile { pat_ = set; }
  void setPat() volatile { setPat(true); }
  void clearPat() volatile { setPat(false); }
  bool global() const volatile { return global_; }
  void setGlobal(bool set) volatile { global_ = set; }
  void setGlobal() volatile { setGlobal(true); }
  void clearGlobal() volatile { setGlobal(false); }
  bool nx() const volatile { return nx_; }
  void setNx(bool set) volatile { nx_ = set; }
  void setNx() volatile { setNx(true); }
  void clearNx() volatile { setNx(false); }

  friend void swap(volatile PageEntry& a, volatile PageEntry& b) {
    using rtk::swap;
    swap(a.value_, b.value_);
  }

 private:
  volatile union __attribute__((packed)) {
    uintptr_t value_;
    struct __attribute__((packed)) {
      bool present_ : 1;
      bool rw_ : 1;
      bool user_ : 1;
      bool pwt_ : 1;
      bool pcd_ : 1;
      bool accessed_ : 1;
      bool dirty_ : 1;
      bool pat_ : 1;
      bool global_ : 1;
      int reserved1_ : 3;
      uintptr_t addressPart_ : 40;
      int reserved2_ : 11;
      bool nx_ : 1;
    };
  };
};  // class PageEntry

enum class PageLevel : int { PT = 1, PD = 2, PDP = 3, PML4 = 4 };

inline PageLevel operator+(PageLevel level, int amount) {
  return static_cast<PageLevel>(static_cast<int>(level) + amount);
}

inline PageLevel operator-(PageLevel level, int amount) {
  return static_cast<PageLevel>(static_cast<int>(level) - amount);
}

inline PageLevel operator++(PageLevel& level, int)  // postfix
{
  PageLevel prev = level;
  level = level + 1;
  return prev;
}

inline PageLevel operator--(PageLevel& level, int)  // postfix
{
  PageLevel prev = level;
  level = level - 1;
  return prev;
}

class __attribute__((aligned(4096))) __attribute__((packed)) PageTable {
 public:
  rtk::StatusOr<volatile PageEntry*> getEntry(size_t index) volatile {
    if (index >= kNumEntries) {
      return rtk::StatusCode::OutOfBounds;
    }

    return &entries_[index];
  }
  rtk::StatusCode setEntry(size_t index, PageEntry in) volatile {
    if (index >= kNumEntries) {
      return rtk::StatusCode::OutOfBounds;
    }

    entries_[index] = in;

    return rtk::StatusCode::Ok;
  }
  void clear() volatile { rtk::memset((void*)this, 0, sizeof(*this)); }

 private:
  static constexpr int kNumEntries = kPageSize / sizeof(PageEntry);
  volatile PageEntry entries_[kNumEntries];
};  // class PageTable

class PageTables {
 public:
  PageTables(const PageTables& other) = delete;
  PageTables(PageTables&& other) = delete;
  PageTables& operator=(const PageTables& other) = delete;
  PageTables& operator=(PageTables&& other) = delete;

  const PageTable& root() { return pml4_; }

  virtual rtk::StatusCode map(uintptr_t vaddr, uintptr_t paddr,
                              PageAttr attributes) const = 0;
  rtk::StatusCode map(size_t count, uintptr_t vaddr, uintptr_t paddr,
                      PageAttr attributes) const {
    for (auto i = 0; i < count; i++) {
      auto status = map(vaddr, paddr, attributes);
      if (status != rtk::StatusCode::Ok) {
        return status;
      }
      vaddr += kPageSize;
      paddr += kPageSize;
    }
    return rtk::StatusCode::Ok;
  }

 protected:
  PageTables(uintptr_t pgtablePaddr, uintptr_t pgtableVaddr)
      : pml4Paddr_{pgtablePaddr}, pml4_{*(PageTable*)pgtableVaddr} {}
  PageTable& pml4_;
  const uintptr_t pml4Paddr_;
};  // class PageTables

class RecursivePageTables final : public PageTables {
 public:
  RecursivePageTables(const KernelMemoryLayout& layout,
                      const PhysicalMemoryAllocator& pallocator,
                      const VirtualMemoryAllocator& vallocator,
                      const MemoryBootstrapper& memoryBootstrapper);

  rtk::StatusCode map(uintptr_t virtAddr, uintptr_t physAddr,
                      PageAttr attributes) const override;

 private:
  const uintptr_t tablesStart_;
  const uintptr_t tablesEnd_;
  const PhysicalMemoryAllocator& pallocator_;
  const VirtualMemoryAllocator& vallocator_;
  constexpr uintptr_t entryAddr(uintptr_t address) const {
    return (tablesStart_ |
            (((uintptr_t)(address) >> 9) &
             ~0x7ULL));  // | works if tables are at least 512gb-aligned
  }
  constexpr size_t entryIndex(uintptr_t addr, PageLevel level) const {
    return ((size_t)((addr >> (12 + ((int)level - 1) * 9)) & 0x1FFULL));
  }
  constexpr uintptr_t tableAddress(uintptr_t vaddr, PageLevel level) const {
    // Address:    [s: sign extend 16 bits 0xFFFF][w: 9 bits][x: 9 bits][y: 9 bits][z: 9 bits][offset: 12 bits]
    // L1 Page table: tablesStart_ + [w: 9 bits][x: 9 bits][y: 9 bits][4kb align: 0x000]
    // L2 Page dir:   tablesStart_ + [111111111][w: 9 bits][x: 9 bits][4kb align: 0x000]
    // L3 PDPT:       tablesStart_ + [111111111][111111111][w: 9 bits][4kb align: 0x000]
    // L4 PML4:       ...            [111111111][111111111][111111111]...
    // Formula:
    //   1. vaddr  &= 0x0000FFFFFFE00000 ([s: 0][w][x][y][z+offset: 0])
    //   2. vaddr >>= 9 * level
    //   3. vaddr  &= 0x0000007FFFFFF000 ([16 bits + 9*level 0][wxy: 27 bits 1, 12 bits 0 - 9*level bits])
    //   4. vaddr  |= 0x0000007FFFFFF000 << 9*(4-level)
    //   5. vaddr  &= 0x0000007FFFFFF000 ([at least 25 bits 0][w,x?,y? up to 27 bits 1][align])
    //   6. vaddr  |= tablesStart_
    //  or,
    //   1. tableAddr = tablesStart_ | ((vaddr >> (9 * level)) | (0x7FFFFFF000 << 9*(4-level))) & 0x7FFFFFF000
    return tablesStart_ | ((vaddr >> (9 * (int)level)) |
                           (0x7FFFFFF000 << (9 * (4 - (int)level)))) &
                              0x7FFFFFF000;
  }
  void mapNoTables(uintptr_t virtAddr, uintptr_t physAddr,
                   PageAttr attributes) const {
    // Assumes PT for entry already exists
    // Uses recursive virtual entry address.
    *(PageEntry*)entryAddr(virtAddr) =
        PageEntry(physAddr, PageAttr::Present | PageAttr::RW);
    invalidate_page(virtAddr);
  }
  void unMapNoInvalidate(uintptr_t virtAddr) {
    *(PageEntry*)entryAddr(virtAddr) = {};
  }
};  // class RecursivePageTables

class PhysicalMemoryAllocator {
 public:
  PhysicalMemoryAllocator(const PhysicalMemoryAllocator& other) = delete;
  PhysicalMemoryAllocator(PhysicalMemoryAllocator&& other) = delete;
  PhysicalMemoryAllocator& operator=(const PhysicalMemoryAllocator& other) =
      delete;
  PhysicalMemoryAllocator& operator=(PhysicalMemoryAllocator&& other) = delete;

  virtual rtk::StatusOr<uintptr_t> allocatePage() const = 0;
  virtual rtk::StatusOr<PageSet> allocatePages(size_t count) const = 0;
  virtual size_t memorySize() const = 0;

 protected:
  PhysicalMemoryAllocator() = default;

 private:
};  // class PhysicalMemoryAllocator

class DefaultPhysicalMemoryAllocator final : public PhysicalMemoryAllocator {
 public:
  DefaultPhysicalMemoryAllocator(MemoryBootstrapper& memoryBootstrapper);
  rtk::StatusOr<uintptr_t> allocatePage() const override;
  rtk::StatusOr<PageSet> allocatePages(size_t count) const override;
  void init(const KernelContext* kernel,
            MemoryBootstrapper& memoryBootstrapper) const;
  size_t memorySize() const override { return memorySize_; }
  rtk::StatusCode initializationStatus() { return initializationStatus_; }

 private:
  const size_t memorySize_;
  mutable size_t bitmapSize_;
  mutable uint8_t* bitmap_;
  mutable size_t lowestFreePage_;
  mutable rtk::StatusCode initializationStatus_ =
      rtk::StatusCode::Uninitialized;
  template <typename T>
  bool alignedFreeCheckAdvanceAndMark(size_t& pageNo, size_t count,
                                      size_t* pagesAllocated) const;
};  // class PhysicalMemoryAllocator

class VirtualMemoryAllocator {
 public:
  VirtualMemoryAllocator(const VirtualMemoryAllocator& other) = delete;
  VirtualMemoryAllocator(VirtualMemoryAllocator&& other) = delete;
  VirtualMemoryAllocator& operator=(const VirtualMemoryAllocator& other) =
      delete;
  VirtualMemoryAllocator& operator=(VirtualMemoryAllocator&& other) = delete;

  virtual rtk::StatusCode allocatePage(
      uintptr_t* newVirtualAddressOut) const = 0;
  virtual rtk::StatusCode allocatePages(
      size_t count, uintptr_t* newVirtualAddressOut) const = 0;

 protected:
  VirtualMemoryAllocator() = default;
};  // class VirtualMemoryAllocator

class DefaultVirtualMemoryAllocator final : public VirtualMemoryAllocator {
 public:
  rtk::StatusCode allocatePage(uintptr_t* newVirtualAddressOut) const override;
  rtk::StatusCode allocatePages(size_t count,
                                uintptr_t* newVirtualAddressOut) const override;
};  // class VirtualMemoryAllocator

class MemoryBootstrapper {
 public:
  MemoryBootstrapper(const MemoryBootstrapper& other) = delete;
  MemoryBootstrapper& operator=(const MemoryBootstrapper& other) = delete;
  MemoryBootstrapper(MemoryBootstrapper&& other) noexcept = delete;
  MemoryBootstrapper& operator=(MemoryBootstrapper&& other) noexcept = delete;

  virtual const PhysicalMemoryAllocator& bootstrapAllocator() const = 0;
  virtual uintptr_t pageTablePhysicalAddress() const = 0;
  virtual size_t memorySize() const = 0;
  virtual uintptr_t mappedVirtualMemoryEnd() const = 0;
  virtual rtk::StatusCode reserveVirtualMemory(size_t pageCount,
                                               uintptr_t* newAddr) = 0;
  virtual const KernelMemoryLayout& layout() const = 0;

  class MemoryRangeSource {
   public:
    virtual bool move_next() = 0;
    virtual const k::PageSet& current() const = 0;
  };  // class MemoryBootstrapper::MemoryRangeSource

  class MemoryRange;
  class MemoryRangeIterator final {
   public:
    MemoryRangeIterator(const MemoryRangeIterator& other) { *this = other; }
    MemoryRangeIterator& operator=(const MemoryRangeIterator& other) = default;
    bool operator!=(const MemoryRangeIterator& other) const {
      return iteration_ != other.iteration_;
    }
    const k::PageSet& operator*() { return value_; }
    MemoryRangeIterator& operator++() {
      if (source_ != nullptr && source_->move_next()) {
        iteration_++;
        value_ = source_->current();
      } else {
        iteration_ = -1;
        value_ = {};
      }
      return *this;
    }

   private:
    friend class MemoryRange;
    MemoryRangeIterator() : source_{nullptr}, iteration_{-1} {}

    MemoryRangeIterator(MemoryRangeSource* source) : MemoryRangeIterator() {
      source_ = source;
      ++*this;
    }
    MemoryRangeSource* source_;
    k::PageSet value_;
    int iteration_;
  };  // class MemoryBootstrapper::MemoryRangeIterator

  class MemoryRange final {
   public:
    MemoryRange(MemoryRangeSource& source) : source_{source} {};
    MemoryRangeIterator begin() { return {&source_}; }
    MemoryRangeIterator end() { return {}; }

   private:
    friend class MemoryBootstrapper;
    MemoryRangeSource& source_;
  };  // class MemoryBootstrapper::MemoryRange

  virtual MemoryRange processFreePhysicalMemoryPages() = 0;

 protected:
  MemoryBootstrapper() = default;
  virtual ~MemoryBootstrapper() = default;
};  // class MemoryBootstrapper
}  // namespace k
