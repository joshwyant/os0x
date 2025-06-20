#pragma once
#include <stdint.h>
#include <stddef.h>
#include "kernel/lib/c/string.h"
#include "kernel/lib/cpp/utility.h"
#include "kernel/asm.h"

namespace k
{
    class KernelMemoryLayout;
    enum class PageAttr : uintptr_t;
    class PageEntry;
    class PageTable;
    class PhysicalMemoryAllocator;
    class VirtualMemoryAllocator;
    class VirtualMemoryMapper;

    constexpr size_t PAGE_SIZE = 0x1000; // 4096 bytes

    class KernelMemoryLayout
    {
    public:
        KernelMemoryLayout(uintptr_t userStart,
                           uintptr_t userEnd,
                           uintptr_t kernelStart,
                           uintptr_t kernelEnd,
                           uintptr_t identityStart,
                           uintptr_t identityEnd,
                           uintptr_t heapStart,
                           uintptr_t heapEnd,
                           uintptr_t kernelImageStart,
                           uintptr_t mmapsStart,
                           uintptr_t mmapsEnd,
                           uintptr_t pgtablesStart,
                           uintptr_t pgtablesEnd,
                           uintptr_t pgtableAddr)
            : _user_start(userStart),
              _user_end(userEnd),
              _kernel_start(kernelStart),
              _kernel_end(kernelEnd),
              _identity_start(identityStart),
              _identity_end(identityEnd),
              _heap_start(heapStart),
              _heap_end(heapEnd),
              _kernel_image_start(kernelImageStart),
              _mmaps_start(mmapsStart),
              _mmaps_end(mmapsEnd),
              _pgtables_start(pgtablesStart),
              _pgtables_end(pgtablesEnd),
              _pgtable_addr(pgtableAddr)
        {
        }
        ~KernelMemoryLayout() = default;
        KernelMemoryLayout(const KernelMemoryLayout &other)
            : _user_start(other._user_start),
              _user_end(other._user_end),
              _kernel_start(other._kernel_start),
              _kernel_end(other._kernel_end),
              _identity_start(other._identity_start),
              _identity_end(other._identity_end),
              _heap_start(other._heap_start),
              _heap_end(other._heap_end),
              _kernel_image_start(other._kernel_image_start),
              _mmaps_start(other._mmaps_start),
              _mmaps_end(other._mmaps_end),
              _pgtables_start(other._pgtables_start),
              _pgtables_end(other._pgtables_end),
              _pgtable_addr(other._pgtable_addr) {};
        KernelMemoryLayout &operator=(const KernelMemoryLayout &) = delete;
        KernelMemoryLayout(KernelMemoryLayout &&) = delete;
        KernelMemoryLayout &operator=(KernelMemoryLayout &&) = delete;
        inline uintptr_t user_space_start() const { return _user_start; }
        inline uintptr_t user_space_end() const { return _user_end; }
        inline uintptr_t kernel_space_start() const { return _kernel_start; }
        inline uintptr_t kernel_space_end() const { return _kernel_end; }
        inline uintptr_t identity_paging_start() const { return _identity_start; }
        inline uintptr_t identity_paging_end() const { return _identity_end; }
        inline uintptr_t heap_start() const { return _heap_start; }
        inline uintptr_t heap_end() const { return _heap_end; }
        inline uintptr_t kernel_image_start() const { return _kernel_image_start; }
        inline uintptr_t memory_maps_start() const { return _mmaps_start; }
        inline uintptr_t memory_maps_end() const { return _mmaps_end; }
        inline uintptr_t page_tables_start() const { return _pgtables_start; }
        inline uintptr_t page_tables_end() const { return _pgtables_end; }
        inline uintptr_t page_table_address() const { return _pgtable_addr; }

    private:
        KernelMemoryLayout() = delete;
        const uintptr_t _user_start;
        const uintptr_t _user_end;
        const uintptr_t _kernel_start;
        const uintptr_t _kernel_end;
        const uintptr_t _identity_start;
        const uintptr_t _identity_end;
        const uintptr_t _heap_start;
        const uintptr_t _heap_end;
        const uintptr_t _kernel_image_start;
        const uintptr_t _mmaps_start;
        const uintptr_t _mmaps_end;
        const uintptr_t _pgtables_start;
        const uintptr_t _pgtables_end;
        const uintptr_t _pgtable_addr;
    };

    class DefaultKernelMemoryLayout : public KernelMemoryLayout
    {
    public:
        DefaultKernelMemoryLayout()
            : KernelMemoryLayout(
                  0x0000000000000000ULL, // user space start:         0
                  0x00007FFFFFFFFFFFULL, // user space end:         128 TiB
                  0xFFFF800000000000ULL, // kernel space start:    -128 TiB
                  0xFFFFFFFFFFFFFFFFULL, // kernel space end:      -
                  0xFFFF800000000000ULL, // identity maps start:   -128 TiB
                  0xFFFF800FFFFFFFFFULL, // identity maps end:     -127 TiB
                  0xFFFF801000000000ULL, // heap start             -127 TiB
                  0xFFFFFEFFFFFFFFFFULL, // heap end               -  1 TiB
                  0xFFFFFF0000000000ULL, // kernel image start     -  1 TiB
                  0xFFFF801000000000ULL, // memory maps start      -  1 TiB
                  0xFFFFFF7FFFFFFFFFULL, // memory maps end        -512 GiB
                  0xFFFFFF8000000000ULL, // page tables start      -512 GiB
                  0xFFFFFFFFFFFFFFFFULL, // page tables end        -
                  0xFFFFFFFFFFFFF000ULL) // page table PML4        -  4 KiB
        {
        }
    };

    enum class PageAttr : uintptr_t
    {
        Present = (1ULL << 0),
        RW = (1ULL << 1),
        User = (1ULL << 2),
        PWT = (1ULL << 3),
        PCD = (1ULL << 4),
        Accessed = (1ULL << 5),
        Dirty = (1ULL << 6),
        PAT = (1ULL << 7),
        Global = (1ULL << 8),
        NX = (1ULL << 63), // Only if EFER.NXE is enabled
    };

    inline PageAttr operator|(PageAttr a, PageAttr b)
    {
        return static_cast<PageAttr>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
    }

    inline PageAttr operator&(PageAttr a, PageAttr b)
    {
        return static_cast<PageAttr>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b));
    }

    inline PageAttr &operator|=(PageAttr &a, PageAttr b)
    {
        return a = a | b;
    }

    inline PageAttr &operator&=(PageAttr &a, PageAttr b)
    {
        return a = a & b;
    }

    inline PageAttr &operator~(PageAttr &a)
    {
        return a = static_cast<PageAttr>(~static_cast<uint64_t>(a));
    }

    class __attribute__((aligned(8))) __attribute__((packed)) PageEntry
    {
    public:
        inline PageEntry()
        {
            value = 0;
        }
        inline PageEntry(uint64_t entry)
        {
            value = entry;
        }
        inline PageEntry(PageEntry &other)
        {
            *this = other;
        }
        inline PageEntry(PageEntry &&other)
        {
            swap(*this, other);
        }
        inline PageEntry(uint64_t page_base, PageAttr attributes)
        {
            value = (page_base & 0x000FFFFFFFFFF000ULL) | static_cast<uint64_t>(attributes);
        }
        inline PageEntry(void *page_base, PageAttr attributes) : PageEntry(reinterpret_cast<uint64_t>(page_base)) {}
        inline PageEntry &operator=(PageEntry &other)
        {
            if (this != &other)
            {
                value = other.value;
            }
            return *this;
        }
        inline PageEntry &operator=(PageEntry &&other)
        {
            if (this != &other)
            {
                swap(*this, other);
            }
            return *this;
        }
        inline PageAttr attributes() const
        {
            return static_cast<PageAttr>(value & 0xFFF0000000000FFFULL);
        }
        inline void clear()
        {
            value = 0;
        }
        inline void setAttributes(PageAttr newAttributes)
        {
            value = value & 0x000FFFFFFFFFF000ULL | static_cast<uint64_t>(newAttributes);
        }
        inline void addAttributes(PageAttr newAttributes)
        {
            value |= static_cast<uint64_t>(newAttributes);
        }
        inline void intersectAttributes(PageAttr otherAttributes)
        {
            setAttributes(attributes() & otherAttributes);
        }
        inline void removeAttributes(PageAttr oldAttributes)
        {
            setAttributes(attributes() & ~oldAttributes);
        }
        inline uint64_t physicalAddress() const
        {
            return address_part << 12;
        }
        inline void setPhysicalAddress(uint64_t address)
        {
            address_part = address >> 12;
        }
        inline void setPhysicalAddress(const void *pointer)
        {
            address_part = reinterpret_cast<uint64_t>(pointer) >> 12;
        }
        inline bool present() const { return present_bit; }
        inline void setPresent(bool set)
        {
            present_bit = set;
        }
        inline void setPresent()
        {
            setPresent(true);
        }
        inline void clearPresent()
        {
            setPresent(false);
        }
        inline bool rw() const { return rw_bit; }
        inline void setRw(bool set) { rw_bit = set; }
        inline void setRw() { setRw(true); }
        inline void clearRw() { setRw(false); }
        inline bool user() const { return user_bit; }
        inline void setUser(bool set) { user_bit = set; }
        inline void setUser() { setUser(true); }
        inline void clearUser() { setUser(false); }
        inline bool pwt() const { return pwt_bit; }
        inline void setPwt(bool set) { pwt_bit = set; }
        inline void setPwt() { setPwt(true); }
        inline void clearPwt() { setPwt(false); }
        inline bool pcd() const { return pcd_bit; }
        inline void setPcd(bool set) { pcd_bit = set; }
        inline void setPcd() { setPcd(true); }
        inline void clearPcd() { setPcd(false); }
        inline bool accessed() const { return accessed_bit; }
        inline void setAccessed(bool set) { accessed_bit = set; }
        inline void setAccessed() { setAccessed(true); }
        inline void clearAccessed() { setAccessed(false); }
        inline bool dirty() const { return dirty_bit; }
        inline void setDirty(bool set) { dirty_bit = set; }
        inline void setDirty() { setDirty(true); }
        inline void clearDirty() { setDirty(false); }
        inline bool pat() const { return pat_bit; }
        inline void setPat(bool set) { pat_bit = set; }
        inline void setPat() { setPat(true); }
        inline void clearPat() { setPat(false); }
        inline bool global() const { return global_bit; }
        inline void setGlobal(bool set) { global_bit = set; }
        inline void setGlobal() { setGlobal(true); }
        inline void clearGlobal() { setGlobal(false); }
        inline bool nx() const { return nx_bit; }
        inline void setNx(bool set) { nx_bit = set; }
        inline void setNx() { setNx(true); }
        inline void clearNx() { setNx(false); }

        friend void swap(PageEntry &a, PageEntry &b)
        {
            using k::swap;
            swap(a.value, b.value);
        }

    private:
        union __attribute__((packed))
        {
            uint64_t value;
            struct __attribute__((packed))
            {
                bool present_bit : 1;
                bool rw_bit : 1;
                bool user_bit : 1;
                bool pwt_bit : 1;
                bool pcd_bit : 1;
                bool accessed_bit : 1;
                bool dirty_bit : 1;
                bool pat_bit : 1;
                bool global_bit : 1;
                int reserved1_bits : 3;
                uint64_t address_part : 40;
                int reserved2_bits : 11;
                bool nx_bit : 1;
            };
        };
    };

    enum class PageLevel : int
    {
        PT = 1,
        PD = 2,
        PDP = 3,
        PML4 = 4
    };

    inline PageLevel operator+(PageLevel level, int amount)
    {
        return static_cast<PageLevel>(static_cast<int>(level) + amount);
    }

    inline PageLevel operator-(PageLevel level, int amount)
    {
        return static_cast<PageLevel>(static_cast<int>(level) - amount);
    }

    inline PageLevel operator++(PageLevel &level, int) // postfix
    {
        PageLevel prev = level;
        level = level + 1;
        return prev;
    }

    inline PageLevel operator--(PageLevel &level, int) // postfix
    {
        PageLevel prev = level;
        level = level - 1;
        return prev;
    }

    class __attribute__((aligned(4096))) __attribute__((packed)) PageTable
    {
    public:
        status_code getEntry(size_t index, PageEntry &out)
        {
            if (index >= num_entries)
            {
                out = {};
                return status_code::out_of_bounds;
            }

            out = entries[index];
            return status_code::ok;
        }
        status_code setEntry(size_t index, PageEntry in)
        {
            if (index >= num_entries)
            {
                return status_code::out_of_bounds;
            }

            entries[index] = in;

            return status_code::ok;
        }
        void clear() volatile
        {
            memset((void *)this, 0, sizeof(*this));
        }

    private:
        static constexpr int num_entries = PAGE_SIZE / sizeof(PageEntry);
        PageEntry entries[num_entries];
    };

    class PageTables
    {
    public:
        PageTables() = delete;
        virtual ~PageTables() = default;
        PageTables(const PageTables &other) = delete;
        virtual PageTables &operator=(const PageTables &other) = delete;
        PageTables(PageTables &&other) noexcept = delete;
        virtual PageTables &operator=(PageTables &&other) noexcept = delete;
        virtual status_code map(uintptr_t vaddr, uintptr_t paddr, PageAttr attributes) = 0;

        PageTable &root()
        {
            return _pml4;
        }

    protected:
        PageTables(uintptr_t pgtable_paddr, uintptr_t pgtable_vaddr)
            : _pml4_paddr(pgtable_paddr), _pml4(*(PageTable *)pgtable_vaddr) {}
        PageTable &_pml4;
        const uintptr_t _pml4_paddr;

    private:
    };

    class RecursivePageTables : public PageTables
    {
    public:
        RecursivePageTables(
            const KernelMemoryLayout &layout,
            const PhysicalMemoryAllocator &pallocator,
            const VirtualMemoryAllocator &vallocator,
            const uintptr_t pgtable_paddr,
            const PageTable *scratchSpace)
            : PageTables(pgtable_paddr, layout.page_table_address()),
              _tables_start(layout.page_tables_start()),
              _tables_end(layout.page_tables_end()),
              _pallocator(pallocator),
              _vallocator(vallocator),
              _scratch_space(scratchSpace)
        {
        }
        virtual status_code map(uintptr_t virt_addr, uintptr_t phys_addr, PageAttr attributes) const;

    protected:
        const uintptr_t _tables_start;
        const uintptr_t _tables_end; // probably wrapped to 0
        const PhysicalMemoryAllocator &_pallocator;
        const VirtualMemoryAllocator &_vallocator;
        const volatile PageTable *_scratch_space;
        inline constexpr uintptr_t entry_addr(uintptr_t address) const
        {
            return (_tables_start | (((uintptr_t)(address) >> 9) & ~0x7ULL)); // | works if tables are at least 512gb-aligned
        }
        inline constexpr size_t entry_index(uintptr_t addr, PageLevel level) const
        {
            return ((size_t)((addr >> (12 + ((int)level - 1) * 9)) & 0x1FFULL));
        }
        inline constexpr uintptr_t table_address(uintptr_t vaddr, PageLevel level) const
        {
            // Address:    [s: sign extend 16 bits 0xFFFF][w: 9 bits][x: 9 bits][y: 9 bits][z: 9 bits][offset: 12 bits]
            // L1 Page table: tables_start + [w: 9 bits][x: 9 bits][y: 9 bits][4kb align: 0x000]
            // L2 Page dir:   tables_start + [111111111][w: 9 bits][x: 9 bits][4kb align: 0x000]
            // L3 PDPT:       tables_start + [111111111][111111111][w: 9 bits][4kb align: 0x000]
            // L4 PML4:       ...            [111111111][111111111][111111111]...
            // Formula:
            //   1. vaddr  &= 0x0000FFFFFFE00000 ([s: 0][w][x][y][z+offset: 0])
            //   2. vaddr >>= 9 * level
            //   3. vaddr  &= 0x0000007FFFFFF000 ([16 bits + 9*level 0][wxy: 27 bits 1, 12 bits 0 - 9*level bits])
            //   4. vaddr  |= 0x0000007FFFFFF000 << 9*(4-level)
            //   5. vaddr  &= 0x0000007FFFFFF000 ([at least 25 bits 0][w,x?,y? up to 27 bits 1][align])
            //   6. vaddr  |= tables_start
            //  or,
            //   1. table_addr = tables_start | ((vaddr >> (9 * level)) | (0x7FFFFFF000 << 9*(4-level))) & 0x7FFFFFF000
            return _tables_start | ((vaddr >> (9 * (int)level)) | (0x7FFFFFF000 << 9 * (4 - (int)level))) & 0x7FFFFFF000;
        }
        virtual inline void map_notables(uintptr_t virt_addr, uintptr_t phys_addr, PageAttr attributes) const
        {
            // Assumes PT for entry already exists
            *(PageEntry *)entry_addr(virt_addr) = PageEntry(phys_addr, PageAttr::Present | PageAttr::RW);
            invalidate_page(virt_addr);
        }
        virtual inline void unmap_noinvalidate(uintptr_t virt_addr)
        {
            *(PageEntry *)entry_addr(virt_addr) = {};
        }
    };

    class PhysicalMemoryAllocator
    {
    public:
        virtual ~PhysicalMemoryAllocator() = delete;
        PhysicalMemoryAllocator(const PhysicalMemoryAllocator &other) = delete;
        virtual PhysicalMemoryAllocator &operator=(const PhysicalMemoryAllocator &other) = delete;
        PhysicalMemoryAllocator(PhysicalMemoryAllocator &&other) noexcept = delete;
        virtual PhysicalMemoryAllocator &operator=(PhysicalMemoryAllocator &&other) noexcept = delete;

        virtual status_code allocate_page(uintptr_t *p_newaddr) const;
        virtual status_code allocate_pages(size_t count, uintptr_t *p_newaddr) const;

    protected:
        PhysicalMemoryAllocator() {}

    private:
    };

    class VirtualMemoryMapper
    {
    public:
        virtual ~VirtualMemoryMapper() = delete;
        VirtualMemoryMapper(const VirtualMemoryMapper &other) = delete;
        virtual VirtualMemoryMapper &operator=(const VirtualMemoryMapper &other) = delete;
        VirtualMemoryMapper(VirtualMemoryMapper &&other) noexcept = delete;
        virtual VirtualMemoryMapper &operator=(VirtualMemoryMapper &&other) noexcept = delete;

    protected:
        VirtualMemoryMapper() {}

    private:
    };
}
