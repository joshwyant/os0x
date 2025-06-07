#pragma once

#include <efi.h>
#include <efilib.h>

#include "boot/bootinfo.h"
#include "efi/mpservice.h"
#include "elf.h"
#include "minc.h"
#include "paging.h"
#include "asm.h"
// #include "miniz.h"

#define STACK_SIZE 0x10000 // 64kb

// Add debug information to a message
#define DEBUGPREFIX(type) __FILE__ ":" LINE_STRING ": " #type ": "
#define Error(msg, ...) Log(ErrorLevel, DEBUGPREFIX(Error) msg, __VA_ARGS__)
#define Info(msg, ...) Log(InfoLevel, msg, __VA_ARGS__)
#define Warning(msg, ...) Log(WarningLevel, DEBUGPREFIX(Warning) msg, __VA_ARGS__)
#define Debug(msg, ...) Log(DebugLevel, msg, __VA_ARGS__)
#define Trace(msg, ...) Log(TraceLevel, msg, __VA_ARGS__)

#define ErrorLine(msg, ...) LogLine(ErrorLevel, DEBUGPREFIX(Error) msg, __VA_ARGS__)
#define InfoLine(msg, ...) LogLine(InfoLevel, msg, __VA_ARGS__)
#define WarningLine(msg, ...) LogLine(WarningLevel, DEBUGPREFIX(Warning) msg, __VA_ARGS__)
#define DebugLine(msg, ...) LogLine(DebugLevel, msg, __VA_ARGS__)
#define TraceLine(msg, ...) LogLine(TraceLevel, msg, __VA_ARGS__)

#define AsciiPrintLine(msg, ...) Print(L##msg "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LogLine(level, msg, ...) Log(level, msg "\n", __VA_ARGS__)
#define LogLine0(level) Log(level##Level, "\n")
#define Log(level, msg, ...)                                                 \
    do                                                                       \
        if ((int)logLevel >= level) /* int to allow -1 to turn off errors */ \
        {                                                                    \
            Print(L##msg __VA_OPT__(, ) __VA_ARGS__);                        \
        }                                                                    \
    while (0)

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)
// Wraps the error checking blocks in the uefi call wrapper
// We use uefi_call_wrapper because we're using gnu-efi, which has to translate the gcc-based calling convention
// to the win32 calls of UEFI.
#define TRYWRAP(tuple, ...) TRYWRAPFN(uefi_call_wrapper tuple, __VA_ARGS__)
#define TRYWRAPS(tuple, msg, ...) TRYWRAPFNS(uefi_call_wrapper tuple, msg, __VA_ARGS__)
// DRY... this is used dozens of times and even widens strings and adds newlines.
#define TRYWRAPFN(fn, ...)                      \
    do                                          \
        if (EFI_ERROR((status = fn)))           \
        {                                       \
            __VA_OPT__(ErrorLine(__VA_ARGS__);) \
            return status;                      \
        }                                       \
    while (0)
// Same as TRYWRAPFN but also adds %r to print out a friendly status string.
#define TRYWRAPFNS(fn, msg, ...) TRYWRAPFN(fn, msg ": %r", __VA_ARGS__ __VA_OPT__(, ) status)
#define TRYEXPR(expr, ret, ...)                 \
    do                                          \
        if (!(expr))                            \
        {                                       \
            __VA_OPT__(ErrorLine(__VA_ARGS__);) \
            return ret;                         \
        }                                       \
    while (0)

#define memset(a, b, c) uefi_call_wrapper(BS->SetMem, 3, a, b, c)

typedef void (*kernel_entry_t)(boot_info_t *bi);
typedef struct
{
    kernel_entry_t entry;
    page_physical_address_t kernel_base;
    virtual_address_t kernel_virtual_base;
    size_t kernel_code_pages;
    size_t kernel_page_count;
    int current_page;
} kernel_image_t;

extern enum BootLogLevel {
    ErrorLevel,
    InfoLevel,
    WarningLevel,
    DebugLevel,
    TraceLevel
} logLevel;

EFI_STATUS create_page_tables(page_table_physical_address_ptr_t page_table_out);
EFI_STATUS map_virtual_address_space(EFI_SYSTEM_TABLE *SystemTable, kernel_image_t *kernel_info, boot_info_t *bi, virtual_address_ptr_t stack_pointer_out, page_table_physical_ptr_t pageTable);
EFI_STATUS load_boot_image(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi);
EFI_STATUS measure_kernel(const void *elf_data, size_t elf_size, kernel_image_t *out);
EFI_STATUS map_kernel(const void *elf_data, size_t elf_size, kernel_image_t *out, page_table_physical_ptr_t pageTable);
EFI_STATUS load_kernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, const void *kernel, size_t kernel_size, boot_info_t *bi);
EFI_STATUS enter_kernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, kernel_image_t *kernel_info, virtual_address_t stack_pointer, page_table_address_t page_table, boot_info_t *bi, UINTN mapKey);
EFI_STATUS get_memmap(EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi, UINTN *mapKey);
EFI_STATUS map_page(page_virtual_address_t virt_addr, page_physical_address_t phys_addr, PageAttributes attrs, page_table_physical_ptr_t pageTable);
EFI_STATUS map_pages(page_virtual_address_t virt_addr, page_physical_address_t phys_addr, PageAttributes attr, int pages, page_table_physical_ptr_t pageTable);
EFI_STATUS map_new_pages(page_virtual_address_t virt_addr, page_physical_address_ptr_t phys_addr_out, PageAttributes attr, int pagesi, page_table_physical_ptr_t pageTable);
EFI_STATUS get_graphics_info(EFI_SYSTEM_TABLE *SystemTable, graphics_info_t *bi);
EFI_STATUS wait_for_key(EFI_SYSTEM_TABLE *SystemTable);
EFI_STATUS get_mp_info(EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi, UINTN *cpuCount);

static void clear_screen(graphics_info_t *gi, uint32_t color)
{
    for (int y = 0; y < gi->framebuffer_height; y++)
    {
        uint32_t *p = (gi->framebuffer_base + y * gi->pixels_per_scanline);
        for (int x = 0; x < gi->framebuffer_width; x++)
        {
            p[x] = color;
        }
    }
}
