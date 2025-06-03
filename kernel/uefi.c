#include <efi.h>
#include "kernel.h"
#include "boot/bootinfo.h"

void clear_screen(uint32_t color, uint32_t *frame_buffer, int width, int height, int pitch);
void parse_memory_map(boot_memmap_t *map_info);

void kernel_boot(boot_info_t *bootInfo)
{
    // Sanity check: basic boot info validation
    if (bootInfo == NULL || bootInfo->magic != BOOTINFO_MAGIC)
        freeze();

    // Make a local copy
    boot_info_t bi = *bootInfo;

    // Do something with the memory map:
    parse_memory_map(&bi.memory_map);

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
            uint64_t size = d->NumberOfPages * 4096;
            // Use base/size for your allocator or memory map
        }
    }
}