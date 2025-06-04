#pragma once

#include <stdint.h>

#define PAGE_ADDR_MASK 0x000FFFFFFFFFF000ULL

#define PAGE_PRESENT (1ULL << 0)
#define PAGE_RW (1ULL << 1)
#define PAGE_USER (1ULL << 2)
#define PAGE_PWT (1ULL << 3)
#define PAGE_PCD (1ULL << 4)
#define PAGE_ACCESSED (1ULL << 5)
#define PAGE_DIRTY (1ULL << 6)
#define PAGE_PAT (1ULL << 7)
#define PAGE_GLOBAL (1ULL << 8)
#define PAGE_NX (1ULL << 63) // Only if EFER.NXE is enabled

// [ 9 bits | 9 bits | 9 bits | 9 bits | 12 bits ]
//   PML4     PDPT     PD       PT       offset

// Indices into the PML4, PML3, PML2, and PML1 tables.
#define PT_L4_IDX(addr) (((uint64_t)(addr) >> 39) & 0x1FFULL)
#define PT_L3_IDX(addr) (((uint64_t)(addr) >> 30) & 0x1FFULL)
#define PT_L2_IDX(addr) (((uint64_t)(addr) >> 21) & 0x1FFULL)
#define PT_L1_IDX(addr) (((uint64_t)(addr) >> 12) & 0x1FFULL)

// Address of the page table maps.
#define PT_BASE 0xFFFFFF8000000000ULL

// Sizes of page table divisions.
#define PT_L1_SIZE 0x0000000000001000ULL
#define PT_L2_SIZE 0x0000000000200000ULL
#define PT_L3_SIZE 0x0000000040000000ULL
#define PT_L4_SIZE 0x0000008000000000ULL

// Base address for tables (inefficient).
#define PT_L1_BASE(addr) (PT_L2_BASE(addr) + PT_L2_IDX(addr) * PT_L1_SIZE)
#define PT_L2_BASE(addr) (PT_L3_BASE(addr) + PT_L3_IDX(addr) * PT_L2_SIZE)
#define PT_L3_BASE(addr) (PT_BASE + PT_L4_IDX(addr) * PT_L3_SIZE)
#define PT_L4_BASE 0xFFFFFFFFFFFFF000ULL

// Addresses for entries in tables (inefficient).
#define PT_L1_ENTRY(addr) (PT_L1_BASE(addr) + PT_L1_IDX(addr) * 8)
#define PT_L2_ENTRY(addr) (PT_L2_BASE(addr) + PT_L2_IDX(addr) * 8)
#define PT_L3_ENTRY(addr) (PT_L3_BASE(addr) + PT_L3_IDX(addr) * 8)
#define PT_L4_ENTRY(addr) (PT_L4_BASE + PT_L4_IDX(addr) * 8)

#define PT_ENTRY(addr) (PT_BASE | (((uint64_t)(addr) >> 9) & ~0x7ULL))
#define PT_PAGE_BASE(pt_entry) ((uint64_t)(pt_entry) & ~0xFFFULL)
static inline void PT_COMPUTE_ENTRIES(uint64_t addr,
                                      uint64_t *ptEntry, uint64_t *pdEntry, uint64_t *pdptEntry, uint64_t *pml4Entry)
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

    // Compute the base address of the page tables easily by truncating to 4096 bits e.g. & ~0xFFF or PT_PAGE_BASE(entry)
}