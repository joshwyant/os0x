
#include "main.h"

#ifdef QUIET
enum BootLogLevel logLevel = TraceLevel;
#else
enum BootLogLevel logLevel = TraceLevel;
#endif

// https://wiki.osdev.org/Debugging_UEFI_applications_with_GDB
// Write a token to 0x10000 to watch in GDB, to find the base
// pointer where EFI loaded us.
// Start Qemu with -s -S, open gdb with:
//   (gdb) watch *(unsigned long long*)0x10000 == 0xDEADBEEF
//   (gdb) continue
//   (gdb) set $base = *(unsigned long long*)0x10008
//   (gdb) add-symbol-file efi/src/main/cpp/bin/BOOTX64.EFI.debug -o $base
//   (gdb) continue
// See .gdbinit
// May have to source manually
// The first "exception" is just the watch being caught
void write_debug_sentinel(EFI_LOADED_IMAGE* LoadedImage) {
  // Write image base and marker for GDB
  volatile uint64_t* marker_ptr = (uint64_t*)0x10000;
  volatile uint64_t* image_base_ptr = (uint64_t*)0x10008;
  *image_base_ptr = (uint64_t)LoadedImage->ImageBase;  // Store ImageBase
  *marker_ptr = 0xDEADBEEF;                            // Set marker
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle,
                           EFI_SYSTEM_TABLE* SystemTable) {
  // disable_lapic();
  EFI_STATUS status;
  graphics_info_t gi;
  EFI_LOADED_IMAGE* LoadedImage;

  InitializeLib(ImageHandle, SystemTable);

  // Get the loaded image protocol from the image handle
  TRYWRAPS(((void*)BS->HandleProtocol, 3, ImageHandle, &LoadedImageProtocol,
            (void**)&LoadedImage),
           "Failed to get LoadedImageProtocol");

  write_debug_sentinel(LoadedImage);

  TRYWRAP(((void*)SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut));
  TRYWRAPFNS(get_graphics_info(SystemTable, &gi),
             "Failed to get graphics info");
  clear_screen(&gi, 0x00181825);  // catppuccin mocha mantle

  boot_info_t* bi;
  TRYWRAP(((void*)SystemTable->BootServices->AllocatePool, 3, EfiLoaderData,
           sizeof(boot_info_t), (void**)&bi));
  memset((void*)bi, 0, sizeof(boot_info_t));

  bi->magic = boot_info_t::BOOTINFO_MAGIC;
  bi->graphics_info = gi;

  InfoLine("os0x, an experimental operating system");
  InfoLine("Copyright (c) 2025 Josh Wyant");

  LogLine0(Debug);
  DebugLine("Press C-t x in the terminal to exit Qemu monitor.");

  LogLine0(Debug);
  DebugLine("Image loaded at: 0x%llp", LoadedImage->ImageBase);

  LogLine0(Info);
  InfoLine("Loading initrd.img...");
  TRYWRAPFNS(load_boot_image(ImageHandle, SystemTable, bi),
             "Failed to load boot image");

  size_t kernel_size;
  const void* kernel;
  TraceLine("Looking for kernel in initrd.img...");
  TRYEXPR(kernel = find_cpio_file((cpio_file_base_ptr_t)(void*)bi->initrd_base,
                                  bi->initrd_size, "kernel.elf", &kernel_size),
          EFI_LOAD_ERROR, "Could not locate kernel.elf in initrd.img!");
  TraceLine("Kernel found.");

  TraceLine("Loading the kernel...");
  TRYWRAPFNS(load_kernel(ImageHandle, SystemTable, kernel, kernel_size, bi),
             "Failed to load the kernel");

  InfoLine("Kernel returned.");
  AsciiPrintLine("Press any key to exit to UEFI...");
  TRYWRAPFNS(wait_for_key(SystemTable), "Failed to read key");

  return EFI_SUCCESS;
}

EFI_STATUS load_boot_image(EFI_HANDLE ImageHandle,
                           EFI_SYSTEM_TABLE* SystemTable, boot_info_t* bi) {
  EFI_STATUS status;
  EFI_LOADED_IMAGE* LoadedImage;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
  EFI_FILE_PROTOCOL* Root;
  EFI_FILE_PROTOCOL* File;

  // Get the loaded image protocol from the image handle
  TRYWRAPS(((void*)BS->HandleProtocol, 3, ImageHandle, &LoadedImageProtocol,
            (void**)&LoadedImage),
           "Failed to get LoadedImageProtocol");

  // Get the Simple File System protocol from the device handle
  TRYWRAPS(((void*)BS->HandleProtocol, 3, LoadedImage->DeviceHandle,
            &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem),
           "Failed to get SimpleFileSystemProtocol");

  // Open the volume (root directory)
  TRYWRAPS(((void*)FileSystem->OpenVolume, 2, FileSystem, &Root),
           "Failed to open volume");

  TRYWRAPS(
      ((void*)Root->Open, 5, Root, &File, L"initrd.img", EFI_FILE_MODE_READ, 0),
      "Failed to open initrd.img");

  EFI_FILE_INFO* fileInfo;
  UINTN fileInfoSize = sizeof(EFI_FILE_INFO) + 200;

  TRYWRAP(((void*)BS->AllocatePool, 3, EfiLoaderData, fileInfoSize,
           (void**)&fileInfo));

  TRYWRAP(((void*)File->GetInfo, 4, File, &gEfiFileInfoGuid, &fileInfoSize,
           fileInfo));

  UINTN fileSize = fileInfo->FileSize;
  void* buffer;
  TRYWRAP(((void*)BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
           EFI_SIZE_TO_PAGES(fileSize), (EFI_PHYSICAL_ADDRESS*)&buffer));

  // Read the file into the buffer
  TRYWRAP(((void*)File->Read, 3, File, &fileSize, buffer));

  // Decompress using miniz
  // Currently, this gives #UD; even when not using zlib deflateBound, or decompress_mem_to_mem.

  // int cmp_status;
  // mz_ulong uncompressed_size = deflateBound(&buffer, fileSize);
  // void *uncompressed_data;
  // TRYWRAP((BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
  //                            EFI_SIZE_TO_PAGES(uncompressed_size), (EFI_PHYSICAL_ADDRESS *)&uncompressed_data));

  // int result = mz_uncompress(uncompressed_data, &uncompressed_size, buffer, fileSize);
  // if (result != MZ_OK)
  // {
  //     ErrorLine("Decompression failed!");
  //     return EFI_LOAD_ERROR;
  // }

  // bi->initrd_base = (uint32_t *)(uintptr_t)uncompressed_data;
  // bi->initrd_size = uncompressed_size;

  bi->initrd_base = (uint32_t*)(uintptr_t)buffer;
  bi->initrd_size = fileSize;

  return EFI_SUCCESS;
}

EFI_STATUS get_graphics_info(EFI_SYSTEM_TABLE* SystemTable,
                             graphics_info_t* gi) {
  EFI_STATUS status;
  EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
  TRYWRAP(((void*)SystemTable->BootServices->LocateProtocol, 3, &gopGuid, NULL,
           (void**)&gop));

  UINT32* framebufferBase = (UINT32*)gop->Mode->FrameBufferBase;
  UINTN framebufferSize = gop->Mode->FrameBufferSize;
  UINT32 width = gop->Mode->Info->HorizontalResolution;
  UINT32 height = gop->Mode->Info->VerticalResolution;
  UINT32 pixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

  gi->framebuffer_base = framebufferBase;
  gi->framebuffer_size = framebufferSize;
  gi->framebuffer_width = width;
  gi->framebuffer_height = height;
  gi->pixels_per_scanline = pixelsPerScanLine;

  return EFI_SUCCESS;
}

EFI_STATUS wait_for_key(EFI_SYSTEM_TABLE* SystemTable) {
  EFI_STATUS status;
  EFI_INPUT_KEY key;
  TRYWRAP(((void*)SystemTable->ConIn->Reset, 2, SystemTable->ConIn, FALSE));

  UINTN index;
  TRYWRAP(((void*)SystemTable->BootServices->WaitForEvent, 3, 1,
           &SystemTable->ConIn->WaitForKey, &index));
  TRYWRAP(
      ((void*)SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &key));

  return EFI_SUCCESS;
}
