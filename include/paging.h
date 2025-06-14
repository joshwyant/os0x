#pragma once

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRY_COUNT 512

typedef uint64_t page_table_entry_t;
typedef page_table_entry_t
    *page_table_entry_ptr_t,
    *page_table_entry_physical_ptr_t,
    *page_table_entry_virtual_ptr_t;
__attribute__((aligned(PAGE_SIZE))) typedef unsigned char page_t[PAGE_SIZE];
typedef page_t
    *page_ptr_t,
    *page_physical_ptr_t,
    *page_virtual_ptr_t;
__attribute__((aligned(PAGE_SIZE))) typedef page_table_entry_t page_table_t[PAGE_TABLE_ENTRY_COUNT];
typedef page_table_t
    *page_table_ptr_t,
    *page_table_physical_ptr_t,
    *page_table_virtual_ptr_t;
typedef uint64_t address_t;
typedef address_t
    physical_address_t,
    virtual_address_t,
    page_address_t,
    page_table_address_t,
    page_table_entry_address_t,
    *address_ptr_t,
    *physical_address_ptr_t,
    *virtual_address_ptr_t;
typedef page_address_t
    page_physical_address_t,
    page_virtual_address_t,
    *page_address_ptr_t,
    *page_physical_address_ptr_t,
    *page_virtual_address_ptr_t;
typedef page_table_address_t
    page_table_physical_address_t,
    page_table_virtual_address_t,
    *page_table_address_ptr_t,
    *page_table_physical_address_ptr_t,
    *page_table_virtual_address_ptr_t;
typedef page_table_entry_address_t
    page_table_entry_physical_address_t,
    page_table_entry_virtual_address_t,
    *page_table_entry_address_ptr_t,
    *page_table_entry_physical_address_ptr_t,
    *page_table_entry_virtual_address_ptr_t;

#define PAGE_ADDR_MASK 0x000FFFFFFFFFF000ULL
#define PAGE_MASK 0xFFFULL

#ifdef __cplusplus
enum class PageAttributes : unsigned long
#else
typedef enum
#endif
{
    PAGE_PRESENT = (1ULL << 0),
    PAGE_RW = (1ULL << 1),
    PAGE_USER = (1ULL << 2),
    PAGE_PWT = (1ULL << 3),
    PAGE_PCD = (1ULL << 4),
    PAGE_ACCESSED = (1ULL << 5),
    PAGE_DIRTY = (1ULL << 6),
    PAGE_PAT = (1ULL << 7),
    PAGE_GLOBAL = (1ULL << 8),
    PAGE_NX = (1ULL << 63), // Only if EFER.NXE is enabled
#ifdef __cplusplus
};

inline PageAttributes operator|(PageAttributes a, PageAttributes b)
{
    return static_cast<PageAttributes>(static_cast<unsigned long long>(a) | static_cast<unsigned long long>(b));
}

inline PageAttributes operator&(PageAttributes a, PageAttributes b)
{
    return static_cast<PageAttributes>(static_cast<unsigned long long>(a) & static_cast<unsigned long long>(b));
}

inline PageAttributes &operator|=(PageAttributes &a, PageAttributes b)
{
    return a = a | b;
}
#else
} PageAttributes;
#endif

// [ 9 bits | 9 bits | 9 bits | 9 bits | 12 bits ]
//   PML4     PDPT     PD       PT       offset

// Indices into the PML4, PML3, PML2, and PML1 tables.
#define PT_L4_IDX(addr) ((size_t)(((virtual_address_t)(addr) >> 39) & 0x1FFULL))
#define PT_L3_IDX(addr) ((size_t)(((virtual_address_t)(addr) >> 30) & 0x1FFULL))
#define PT_L2_IDX(addr) ((size_t)(((virtual_address_t)(addr) >> 21) & 0x1FFULL))
#define PT_L1_IDX(addr) ((size_t)(((virtual_address_t)(addr) >> 12) & 0x1FFULL))
#define PT_IDX(addr, level) ((size_t)(((virtual_address_t)(addr) >> (12 + (int)(level - 1) * 9)) & 0x1FFULL))

// Address of the page table maps.
#define PT_BASE ((page_table_virtual_address_t)0xFFFFFF8000000000ULL)
#define PT_BASE_PTR ((page_table_virtual_ptr_t)PT_BASE)

// Sizes of page table divisions.
#define PT_L1_SIZE ((size_t)0x0000000000001000ULL)
#define PT_L2_SIZE ((size_t)0x0000000000200000ULL)
#define PT_L3_SIZE ((size_t)0x0000000040000000ULL)
#define PT_L4_SIZE ((size_t)0x0000008000000000ULL)

// Base address for tables (inefficient).
#define PT_L1_BASE(addr) ((page_table_virtual_address_t)(PT_L2_BASE((virtual_address_t)(addr)) + PT_L2_IDX((virtual_address_t)(addr)) * PT_L1_SIZE))
#define PT_L2_BASE(addr) ((page_table_virtual_address_t)(PT_L3_BASE((virtual_address_t)(addr)) + PT_L3_IDX((virtual_address_t)(addr)) * PT_L2_SIZE))
#define PT_L3_BASE(addr) ((page_table_virtual_address_t)(PT_BASE + PT_L4_IDX((virtual_address_t)(addr)) * PT_L3_SIZE))
#define PT_L4_BASE ((page_table_virtual_address_t)0xFFFFFFFFFFFFF000ULL)
#define PT_L1_BASE_PTR(addr) ((page_table_virtual_ptr_t)PT_L1_BASE((virtual_address_t)(addr)))
#define PT_L2_BASE_PTR(addr) ((page_table_virtual_ptr_t)PT_L2_BASE((virtual_address_t)(addr)))
#define PT_L3_BASE_PTR(addr) ((page_table_virtual_ptr_t)PT_L3_BASE((virtual_address_t)(addr)))
#define PT_L4_BASE_PTR ((page_table_virtual_ptr_t)PT_L4_BASE)

// Addresses for entries in tables (inefficient).
#define PT_L1_ENTRY(addr) ((page_table_entry_virtual_address_t)(PT_L1_BASE((virtual_address_t)(addr)) + PT_L1_IDX((virtual_address_t)(addr)) * 8))
#define PT_L2_ENTRY(addr) ((page_table_entry_virtual_address_t)(PT_L2_BASE((virtual_address_t)(addr)) + PT_L2_IDX((virtual_address_t)(addr)) * 8))
#define PT_L3_ENTRY(addr) ((page_table_entry_virtual_address_t)(PT_L3_BASE((virtual_address_t)(addr)) + PT_L3_IDX((virtual_address_t)(addr)) * 8))
#define PT_L4_ENTRY(addr) ((page_table_entry_virtual_address_t)(PT_L4_BASE + PT_L4_IDX((virtual_address_t)(addr)) * 8))

// Addresses for entries in tables (inefficient).
#define PT_L1_ENTRY_PTR(addr) ((page_table_entry_virtual_ptr_t)PT_L1_ENTRY((virtual_address_t)(addr)))
#define PT_L2_ENTRY_PTR(addr) ((page_table_entry_virtual_ptr_t)PT_L2_ENTRY((virtual_address_t)(addr)))
#define PT_L3_ENTRY_PTR(addr) ((page_table_entry_virtual_ptr_t)PT_L3_ENTRY((virtual_address_t)(addr)))
#define PT_L4_ENTRY_PTR(addr) ((page_table_entry_virtual_ptr_t)PT_L4_ENTRY((virtual_address_t)(addr)))

#define PT_ENTRY(addr) ((page_table_entry_virtual_address_t)(PT_BASE | (((virtual_address_t)(addr) >> 9) & ~0x7ULL)))
#define PT_ENTRY_PTR(addr) ((page_table_entry_virtual_ptr_t)PT_ENTRY((virtual_address_t)(addr)))
#define PT_PAGE_BASE(pt_entry) ((page_table_virtual_address_t)((page_table_entry_virtual_address_t)(pt_entry) & ~0xFFFULL))
#define PT_PAGE_BASE_PTR(pt_entry) ((page_table_virtual_ptr_t)PT_PAGE_BASE((page_table_entry_virtual_address_t)pt_entry))
static inline void PT_COMPUTE_ENTRIES(virtual_address_t addr,
                                      page_table_entry_virtual_address_ptr_t ptEntry,
                                      page_table_entry_virtual_address_ptr_t pdEntry,
                                      page_table_entry_virtual_address_ptr_t pdptEntry,
                                      page_table_entry_virtual_address_ptr_t pml4Entry)
{
    // // Use only 48 bits of address
    // addr &= 0x0000FFFFFFFFFFFF;
    // // Divide by 4096 to get index into page table maps
    // uint16_t maps_idx = addr >> 12;
    // // Multiply by 8 to get address of entry into page table maps
    // uint16_t maps_entry = maps_idx << 3;
    // also add PT_BASE
    // // That's equal to:
    // *ptEntry = PT_BASE + (addr >> 9) & 0x0000007FFFFFFFF8ULL;
    // // which is the same as
    // *ptEntry = PT_BASE | (addr >> 9) & ~0x7ULL;
    *ptEntry = PT_ENTRY(addr);
    // Recursively find the higher level entry
    *pdEntry = PT_ENTRY(*ptEntry);
    *pdptEntry = PT_ENTRY(*pdEntry);
    *pml4Entry = PT_ENTRY(*pdptEntry);

    // Compute the base address of the page tables easily by truncating to multiple of 4096 bytes e.g. & ~0xFFF or PT_PAGE_BASE(entry)
}