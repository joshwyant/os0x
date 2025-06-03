#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    Print(L"Hello from UEFI!\n");
    Print(L"Press C-t x to exit Qemu monitor.\n");
    Print(L"Press any key to exit to UEFI...\n");

    EFI_INPUT_KEY key;
    uefi_call_wrapper(SystemTable->ConIn->Reset, 2, SystemTable->ConIn, FALSE);
    UINTN index;
    uefi_call_wrapper(SystemTable->BootServices->WaitForEvent, 3,
                      1, &SystemTable->ConIn->WaitForKey, &index);
    uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2,
                      SystemTable->ConIn, &key);

    return EFI_SUCCESS;
}