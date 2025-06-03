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

    kernel_main();

    // Loop forever
    while (TRUE)
        halt_cpu();
}