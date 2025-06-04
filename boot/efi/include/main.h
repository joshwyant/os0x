#pragma once

#include <efi.h>
#include <efilib.h>

#include "boot/bootinfo.h"
#include "elf.h"
#include "minc.h"
// #include "miniz.h"

#define Info(...)               \
    do                          \
        if (info_on)            \
        {                       \
            Print(__VA_ARGS__); \
        }                       \
    while (0);
extern int info_on;

typedef void (*kernel_entry_t)(boot_info_t *bi);
typedef struct
{
    kernel_entry_t entry;
    void *kernel_base;
    size_t kernel_size;
} kernel_image_t;

EFI_STATUS load_boot_image(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi);
EFI_STATUS load_kernel(void *elf_data, size_t elf_size, kernel_image_t *out);
EFI_STATUS enter_kernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, kernel_image_t *kernel_info, boot_info_t *bi, UINTN mapKey);
EFI_STATUS get_memmap(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, kernel_image_t *kernel_info, boot_info_t *bi, UINTN *mapKey);
void fill_graphics_info(EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi);
void clear_screen(boot_info_t *bi, uint32_t color);
void wait_for_key(EFI_SYSTEM_TABLE *SystemTable);
