#include "kernel.h"
#include "boot/bootinfo.h"

void clear_screen(uint32_t color, uint32_t *frame_buffer, int width, int height, int pitch);

void kernel_boot(boot_info_t *bootInfo)
{
    // Sanity check: basic boot info validation
    if (bootInfo == NULL || bootInfo->magic != BOOTINFO_MAGIC)
    {
        // Halt if invalid
        while (TRUE)
            halt_cpu();
    }

    clear_screen(0x00000000, // black
                 bootInfo->framebuffer_base,
                 bootInfo->framebuffer_width,
                 bootInfo->framebuffer_height,
                 bootInfo->pixels_per_scanline);

    kernel_main();

    // Loop forever
    while (TRUE)
        halt_cpu();
}

void clear_screen(uint32_t color, uint32_t *frame_buffer, int width, int height, int pitch)
{
    for (int y = 0; y < height; y++)
    {
        uint32_t *p = (frame_buffer + y * pitch);
        for (int x = 0; x < width; x++)
        {
            p[x] = color; // Black
        }
    }
}