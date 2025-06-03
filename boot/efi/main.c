#include <efi.h>
#include <efilib.h>

#include "boot/bootinfo.h"
#include "elf.h"
#include "minc.h"
// #include "miniz.h"

#ifdef QUIET
int info_on = FALSE;
#else
int info_on = TRUE;
#endif
#define Info(...)               \
    do                          \
        if (info_on)            \
        {                       \
            Print(__VA_ARGS__); \
        }                       \
    while (0);

typedef void (*kernel_entry_t)(boot_info_t *bi);

typedef struct
{
    kernel_entry_t entry;
    void *kernel_base;
    size_t kernel_size;
} kernel_image_t;

EFI_STATUS load_boot_image(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi);
EFI_STATUS load_kernel(void *elf_data, size_t elf_size, kernel_image_t *out);
EFI_STATUS enter_kernel(EFI_HANDLE ImageHandle, kernel_image_t *kernel_info, boot_info_t *bi);
void fill_graphics_info(EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi);
void clear_screen(boot_info_t *bi, uint32_t color);
void wait_for_key(EFI_SYSTEM_TABLE *SystemTable);

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS status;

    InitializeLib(ImageHandle, SystemTable);

    Info(L"os0x, an experimental operating system\n");
    Info(L"Copyright (c) 2025 Josh Wyant\n");
    Info(L"\nPress C-t x to exit Qemu monitor.\n\n");

    boot_info_t bi = {
        .magic = BOOTINFO_MAGIC,
    };
    fill_graphics_info(SystemTable, &bi);

    clear_screen(&bi, 0x00181825); // catppuccin mocha mantle

    Info(L"Loading initrd.img...\n");
    status = load_boot_image(ImageHandle, SystemTable, &bi);
    if (EFI_ERROR(status))
    {
        return status;
    }

    size_t kernel_size;
    void *kernel = find_cpio_file(bi.initrd_base, bi.initrd_size, "kernel.elf", &kernel_size);
    if (kernel == NULL)
    {
        Print(L"Could not locate kernel.elf in initrd.img!\n");
        return EFI_LOAD_ERROR;
    }

    kernel_image_t kernel_info;
    status = load_kernel(kernel, kernel_size, &kernel_info);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to load the kernel: %r\n", status);
        return status;
    }

    status = enter_kernel(ImageHandle, &kernel_info, &bi);
    if (EFI_ERROR(status))
    {
        return status;
    }

    Info(L"Kernel returned.\n");
    Print(L"Press any key to exit to UEFI...\n");
    wait_for_key(SystemTable);

    return EFI_SUCCESS;
}

EFI_STATUS load_boot_image(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi)
{
    // We use uefi_call_wrapper because we're using gnu-efi, which has to translate the gcc-based calling convention
    // to the win32 calls of UEFI.

    EFI_STATUS status;
    EFI_LOADED_IMAGE *LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
    EFI_FILE_PROTOCOL *Root;
    EFI_FILE_PROTOCOL *File;

    // Get the loaded image protocol from the image handle
    status = uefi_call_wrapper(BS->HandleProtocol, 3,
                               ImageHandle,
                               &LoadedImageProtocol,
                               (void **)&LoadedImage);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to get LoadedImageProtocol: %r\n", status);
        return status;
    }

    // Get the Simple File System protocol from the device handle
    status = uefi_call_wrapper(BS->HandleProtocol, 3,
                               LoadedImage->DeviceHandle,
                               &gEfiSimpleFileSystemProtocolGuid,
                               (void **)&FileSystem);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to get SimpleFileSystemProtocol: %r\n", status);
        return status;
    }

    // Open the volume (root directory)
    status = uefi_call_wrapper(FileSystem->OpenVolume, 2,
                               FileSystem,
                               &Root);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to open volume: %r\n", status);
        return status;
    }

    status = uefi_call_wrapper(Root->Open, 5,
                               Root,
                               &File,
                               L"initrd.img",
                               EFI_FILE_MODE_READ,
                               0);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to open initrd.img: %r\n", status);
        return status;
    }

    EFI_FILE_INFO *fileInfo;
    UINTN fileInfoSize = sizeof(EFI_FILE_INFO) + 200;

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, fileInfoSize, (void **)&fileInfo);
    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(File->GetInfo, 4, File, &gEfiFileInfoGuid, &fileInfoSize, fileInfo);
    if (EFI_ERROR(status))
        return status;

    UINTN fileSize = fileInfo->FileSize;
    void *buffer;
    status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
                               EFI_SIZE_TO_PAGES(fileSize), (EFI_PHYSICAL_ADDRESS *)&buffer);
    if (EFI_ERROR(status))
        return status;

    // Read the file into the buffer
    status = uefi_call_wrapper(File->Read, 3, File, &fileSize, buffer);
    if (EFI_ERROR(status))
        return status;

    // Decompress using miniz
    // Currently, this gives #UD; even when not using zlib deflateBound, or decompress_mem_to_mem.

    // int cmp_status;
    // mz_ulong uncompressed_size = deflateBound(&buffer, fileSize);
    // void *uncompressed_data;
    // status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
    //                            EFI_SIZE_TO_PAGES(uncompressed_size), (EFI_PHYSICAL_ADDRESS *)&uncompressed_data);
    // if (EFI_ERROR(status))
    //     return status;

    // int result = mz_uncompress(uncompressed_data, &uncompressed_size, buffer, fileSize);
    // if (result != MZ_OK)
    // {
    //     Print(L"Decompression failed!\n");
    //     return EFI_LOAD_ERROR;
    // }

    // bi->initrd_base = (uint32_t *)(uintptr_t)uncompressed_data;
    // bi->initrd_size = uncompressed_size;

    bi->initrd_base = (uint32_t *)(uintptr_t)buffer;
    bi->initrd_size = fileSize;

    return EFI_SUCCESS;
}

void wait_for_key(EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_INPUT_KEY key;
    uefi_call_wrapper(SystemTable->ConIn->Reset, 2, SystemTable->ConIn, FALSE);
    UINTN index;
    uefi_call_wrapper(SystemTable->BootServices->WaitForEvent, 3,
                      1, &SystemTable->ConIn->WaitForKey, &index);
    uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2,
                      SystemTable->ConIn, &key);
}

void fill_graphics_info(EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi)
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3, &gopGuid, NULL, (void **)&gop);

    UINT32 *framebufferBase = (UINT32 *)gop->Mode->FrameBufferBase;
    UINTN framebufferSize = gop->Mode->FrameBufferSize;
    UINT32 width = gop->Mode->Info->HorizontalResolution;
    UINT32 height = gop->Mode->Info->VerticalResolution;
    UINT32 pixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

    bi->framebuffer_base = framebufferBase;
    bi->framebuffer_size = framebufferSize;
    bi->framebuffer_width = width;
    bi->framebuffer_height = height;
    bi->pixels_per_scanline = pixelsPerScanLine;
}

EFI_STATUS load_kernel(void *elf_data, size_t elf_size, kernel_image_t *out)
{
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_data;
    unsigned char elfmag[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
    if (*(uint32_t *)ehdr->e_ident != *(uint32_t *)elfmag)
        return EFI_LOAD_ERROR;

    Elf64_Phdr *phdrs = (Elf64_Phdr *)((uint8_t *)elf_data + ehdr->e_phoff);
    for (int i = 0; i < ehdr->e_phnum; ++i)
    {
        Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD)
            continue;

        EFI_PHYSICAL_ADDRESS segment_addr = (UINT64)ph->p_paddr;
        UINTN pages = EFI_SIZE_TO_PAGES(ph->p_memsz);

        EFI_STATUS status = uefi_call_wrapper(BS->AllocatePages, 4,
                                              AllocateAddress, EfiLoaderData,
                                              pages, &segment_addr);
        if (EFI_ERROR(status))
            return status;

        _memcpy((void *)(uintptr_t)segment_addr,
                (uint8_t *)elf_data + ph->p_offset,
                ph->p_filesz);

        if (ph->p_memsz > ph->p_filesz)
            _memset((uint8_t *)segment_addr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
    }

    out->entry = (void *)(uintptr_t)ehdr->e_entry;
    out->kernel_base = (void *)(uintptr_t)phdrs[0].p_paddr;
    out->kernel_size = phdrs[0].p_memsz;
    return EFI_SUCCESS;
}

EFI_STATUS enter_kernel(EFI_HANDLE ImageHandle, kernel_image_t *kernel_info, boot_info_t *bi)
{
    EFI_STATUS status;
    // UINTN map_key, desc_size;
    // UINT32 desc_version;
    // status = uefi_call_wrapper(BS->GetMemoryMap, ...); // standard procedure
    // if (EFI_ERROR(status))
    // {
    //     Print(L"Could not get memory map: %r\n", status);
    //     return status;
    // }
    // status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, map_key);
    // if (EFI_ERROR(status))
    // {
    //     Print(L"Could not exit boot services: %r\n", status);
    //     return status;
    // }

    Info(L"Kernel loaded. Executing...\n");
    kernel_info->entry(bi);

    // Should kernel return
    return EFI_SUCCESS;
}

void clear_screen(boot_info_t *bi, uint32_t color)
{
    for (int y = 0; y < bi->framebuffer_height; y++)
    {
        uint32_t *p = (bi->framebuffer_base + y * bi->pixels_per_scanline);
        for (int x = 0; x < bi->framebuffer_width; x++)
        {
            p[x] = color;
        }
    }
}