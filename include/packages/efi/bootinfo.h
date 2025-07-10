#pragma once

#include <efi.h>
#include <stddef.h>
#include <stdint.h>
#include "packages/efi/minc.h"
#include "packages/efi/paging.h"

typedef struct {
  EFI_MEMORY_DESCRIPTOR* memory_map;  // Pointer to UEFI memory map
  uint64_t memory_map_size;           // Total size in bytes
  uint64_t descriptor_size;           // Size of each descriptor
  uint32_t descriptor_version;
} boot_memmap_t;

typedef struct {
  // Frame buffer
  uint32_t* framebuffer_base;
  uint32_t* framebuffer_virtual_base;
  uint64_t framebuffer_size;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint32_t pixels_per_scanline;
} graphics_info_t;

typedef struct {
  enum { BOOTINFO_MAGIC = 0x1BADB002 } magic;
  // InitRD Image
  uint32_t* initrd_base;
  size_t initrd_size;
  // Page table physical address
  page_table_physical_address_t page_table_physical;
  // Page table virtual address
  page_table_virtual_address_t page_table_virtual;
  // Graphics info
  graphics_info_t graphics_info;
  // Memory Map
  boot_memmap_t memory_map;
  // Next free address
  virtual_address_t memory_end;
  // Stack info
  uint64_t cpu_count;
  page_ptr_t stack_area_base;
  uint64_t stack_pages_per_cpu;
  // IDT
  virtual_address_t idt_addr;
  // Add more fields as needed (e.g., memory map, ACPI, etc.)
} boot_info_t;

typedef const char* cpio_file_entry_ptr_t;
typedef const char* cpio_file_base_ptr_t;

static inline cpio_file_entry_ptr_t find_cpio_file(cpio_file_base_ptr_t base,
                                                   size_t size,
                                                   const char* filename,
                                                   size_t* out_size) {
  const char* p = base;
  const char* end = p + size;

  while (p + 110 <= end) {
    if (memcmp(p, "070701", 6) != 0)
      break;

    uint32_t namesize = strtoul(p + 94, NULL, 16);
    uint32_t filesize = strtoul(p + 54, NULL, 16);

    const char* name = p + 110;
    const char* data = (const char*)(((uintptr_t)(name + namesize) + 3) & ~3);

    if (strncmp(name, filename, namesize - 1) == 0) {
      if (out_size)
        *out_size = filesize;
      return data;
    }

    const char* next = (const char*)(((uintptr_t)(data + filesize) + 3) & ~3);
    p = next;
  }

  return NULL;
}