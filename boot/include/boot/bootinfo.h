#pragma once

#include <stddef.h>
#include <stdint.h>
#include "minc.h"

typedef struct
{
    enum
    {
        BOOTINFO_MAGIC = 0x1BADB002
    } magic;
    uint32_t *initrd_base;
    size_t initrd_size;
    uint32_t *framebuffer_base;
    uint64_t framebuffer_size;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_pitch;
    uint32_t pixels_per_scanline;

    // Add more fields as needed (e.g., memory map, ACPI, etc.)
} boot_info_t;

static void *find_cpio_file(void *base, size_t size, const char *filename, size_t *out_size)
{
    const char *p = base;
    const char *end = p + size;

    while (p + 110 <= end)
    {
        if (_memcmp(p, "070701", 6) != 0)
            break;

        uint32_t namesize = _strtoul(p + 94);
        uint32_t filesize = _strtoul(p + 54);

        const char *name = p + 110;
        const char *data = (const char *)(((uintptr_t)(name + namesize) + 3) & ~3);

        if (_strncmp(name, filename, namesize - 1) == 0)
        {
            if (out_size)
                *out_size = filesize;
            return (void *)data;
        }

        const char *next = (const char *)(((uintptr_t)(data + filesize) + 3) & ~3);
        p = next;
    }

    return NULL;
}