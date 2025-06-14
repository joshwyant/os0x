#include <efi.h>
#include "kernel/kernel.h"
#include "boot/bootinfo.h"

static void clear_screen(boot_info_t *bi, uint32_t color);
void parse_memory_map(boot_memmap_t *map_info);

void kernel_boot(boot_info_t *bootInfo)
{
    // Sanity check: basic boot info validation
    if (bootInfo == NULL || bootInfo->magic != BOOTINFO_MAGIC)
        freeze();

    clear_screen(bootInfo, 0x00181825); // catppuccin mocha mantle
    for (;;)
        ;

    // Do something with the memory map:
    parse_memory_map(&bootInfo->memory_map);

    kernel_main();

    // Loop forever
    freeze();
}

void parse_memory_map(boot_memmap_t *map_info)
{
    uint8_t *desc = (uint8_t *)map_info->memory_map;
    for (UINTN i = 0; i < map_info->memory_map_size; i += map_info->descriptor_size)
    {
        EFI_MEMORY_DESCRIPTOR *d = (EFI_MEMORY_DESCRIPTOR *)(desc + i);

        // For example:
        if (d->Type == EfiConventionalMemory)
        {
            // Available RAM block
            uint64_t base = d->PhysicalStart;
            uint64_t size = d->NumberOfPages * EFI_PAGE_SIZE;
            // Use base/size for your allocator or memory map
        }
    }
}

static void clear_screen(boot_info_t *bi, uint32_t color)
{
    for (int y = 0; y < bi->graphics_info.framebuffer_height; y++)
    {
        uint32_t *p = (bi->graphics_info.framebuffer_virtual_base + y * bi->graphics_info.pixels_per_scanline);
        for (int x = 0; x < bi->graphics_info.framebuffer_width; x++)
        {
            p[x] = color;
        }
    }
}