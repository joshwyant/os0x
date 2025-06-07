#include "main.h"

EFI_STATUS load_kernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, const void *kernel, size_t kernel_size, boot_info_t *bi)
{
    EFI_STATUS status;
    kernel_image_t kernel_info;
    page_table_physical_ptr_t pageTable;

    TraceLine("Measuring kernel size...");
    TRYWRAPFNS(measure_kernel(kernel, kernel_size, &kernel_info),
               "Failed to measure the kernel size");

    TraceLine("Creating page tables...");
    TRYWRAPFNS(create_page_tables((page_table_physical_address_t *)&pageTable),
               "Failed to create page tables");

    virtual_address_t stack_pointer;
    TraceLine("Mapping virtual address space...");
    TRYWRAPFNS(map_virtual_address_space(SystemTable, &kernel_info, bi, &stack_pointer, pageTable),
               "Failed to map virtual address space");

    TraceLine("Mapping the kernel into virtual memory...");
    TRYWRAPFNS(map_kernel(kernel, kernel_size, &kernel_info, pageTable),
               "Failed to map the kernel into virtual memory");

    UINTN mapKey;
    TraceLine("Getting system memory map...");
    TRYWRAPFNS(get_memmap(SystemTable, bi, &mapKey),
               "Failed to get memory map");

    TraceLine("Calling kernel entry point...");
    TRYWRAPFNS(enter_kernel(ImageHandle, SystemTable, &kernel_info, stack_pointer, (page_table_physical_address_t)pageTable, bi, mapKey),
               "Failed to call kernel entry point");

    return EFI_SUCCESS;
}

EFI_STATUS enter_kernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, kernel_image_t *kernel_info, virtual_address_t stack_pointer, page_table_physical_address_t page_table, boot_info_t *bi, UINTN mapKey)
{
    EFI_STATUS status;
    TRYWRAPS((SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mapKey),
             "Could not exit boot services");

    InfoLine("Kernel loaded. Executing...");
    trampoline(page_table, stack_pointer, (physical_address_t)bi, (virtual_address_t)kernel_info->entry);

    // Should kernel return
    return EFI_SUCCESS;
}

EFI_STATUS map_virtual_address_space(EFI_SYSTEM_TABLE *SystemTable, kernel_image_t *kernel_info, boot_info_t *bi, virtual_address_ptr_t stack_pointer_out, page_table_physical_ptr_t pageTable)
{
    EFI_STATUS status;
    UINTN cpuCount;

    // Map in the boot info as a physical location, and update boot info
    page_physical_address_t boot_info_start = (page_physical_address_t)bi & ~PAGE_MASK;
    page_physical_address_t boot_info_end = ((page_physical_address_t)bi + sizeof(boot_info_t) + PAGE_MASK) & ~PAGE_MASK;
    size_t boot_info_pages = (boot_info_start - boot_info_end) >> 12;
    TRYWRAPFN(map_pages(boot_info_start, boot_info_start, PAGE_PRESENT | PAGE_RW | PAGE_NX, boot_info_pages, pageTable));

    // Skip past the kernel image sections for now, those need special page permissions
    page_virtual_address_t next_page = kernel_info->kernel_virtual_base + kernel_info->kernel_page_count * EFI_PAGE_SIZE;

    // Map in the stacks
    stack_pointer_out = (virtual_address_ptr_t)(next_page + EFI_PAGE_SIZE // guard page
                                                + STACK_SIZE);            // Top of the stack
    int stack_pages = EFI_SIZE_TO_PAGES(STACK_SIZE);
    TRYWRAPFNS(get_mp_info(SystemTable, bi, &cpuCount),
               "Failed to get CPU count");
    for (int i = 0; i < cpuCount; i++)
    {
        next_page += EFI_PAGE_SIZE; // guard page
        page_physical_address_t stack_addr;
        TRYWRAPFN(map_new_pages(next_page, &stack_addr, PAGE_PRESENT | PAGE_RW | PAGE_NX, stack_pages, pageTable));

        // Zero it out
        uefi_call_wrapper(BS->SetMem, 3, (EFI_PHYSICAL_ADDRESS *)&stack_addr, STACK_SIZE, 0);
        next_page += STACK_SIZE;
    }

    // Map in the frame buffer
    int framebuf_pages = EFI_SIZE_TO_PAGES(bi->graphics_info.framebuffer_size);
    bi->graphics_info.framebuffer_virtual_base = (uint32_t *)next_page;
    TRYWRAPFN(map_pages((page_virtual_address_t)bi->graphics_info.framebuffer_virtual_base,
                        (page_physical_address_t)bi->graphics_info.framebuffer_base,
                        PAGE_PRESENT | PAGE_RW | PAGE_NX,
                        framebuf_pages, pageTable));
    next_page += framebuf_pages * EFI_PAGE_SIZE;

    // Map in initrd image
    int initrd_pages = EFI_SIZE_TO_PAGES(bi->initrd_size);
    TRYWRAPFN(map_pages(next_page, (page_physical_address_t)bi->initrd_base, PAGE_PRESENT | PAGE_RW | PAGE_NX, initrd_pages, pageTable));
    bi->initrd_base = (uint32_t *)next_page; // physical to virtual
    next_page += initrd_pages;

    return EFI_SUCCESS;
}

EFI_STATUS map_kernel(const void *elf_data, size_t elf_size, kernel_image_t *out, page_table_physical_ptr_t pageTable)
{
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_data;
    uint8_t elfmag[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
    if (_memcmp(ehdr->e_ident, elfmag, sizeof(elfmag)))
        return EFI_LOAD_ERROR;

    Elf64_Phdr *phdrs = (Elf64_Phdr *)((uint8_t *)elf_data + ehdr->e_phoff);
    out->kernel_page_count = 0;
    for (int i = 0; i < ehdr->e_phnum; ++i)
    {
        Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD)
            continue;

        out->kernel_page_count += EFI_SIZE_TO_PAGES(ph->p_memsz);
    }
    EFI_STATUS status;
    TRYWRAP((BS->AllocatePages, 4,
             AllocateAnyPages, EfiLoaderData,
             out->kernel_page_count, (EFI_PHYSICAL_ADDRESS *)&out->kernel_base));

    int page = 0;
    for (int i = 0; i < ehdr->e_phnum; ++i)
    {
        Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD)
            continue;

        UINTN pages = EFI_SIZE_TO_PAGES(ph->p_memsz);

        page_virtual_address_t segment_addr = (page_virtual_address_t)out->kernel_base + page * EFI_PAGE_SIZE;
        page_physical_address_t physaddr;
        PageAttributes attr = PAGE_PRESENT;
        if (!(ph->p_flags & PF_X))
        {
            attr |= PAGE_NX;
        }
        if ((ph->p_flags) & PF_W)
        {
            attr |= PAGE_RW;
        }
        TRYWRAPFN(map_new_pages(segment_addr, &physaddr, attr, pages, pageTable));

        memcpy((void *)physaddr,
               (uint8_t *)elf_data + ph->p_offset,
               ph->p_filesz);

        if (ph->p_memsz > ph->p_filesz)
            memset((uint8_t *)physaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);

        page += pages;
    }

    out->entry = (void *)(uintptr_t)ehdr->e_entry;
    out->kernel_virtual_base = (virtual_address_t)phdrs[0].p_vaddr;
    out->kernel_code_pages = EFI_SIZE_TO_PAGES(phdrs[0].p_memsz);
    out->kernel_page_count = page;
    return EFI_SUCCESS;
}

EFI_STATUS measure_kernel(const void *elf_data, size_t elf_size, kernel_image_t *out)
{
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_data;
    uint8_t elfmag[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
    if (_memcmp(ehdr->e_ident, elfmag, sizeof(elfmag)))
    {
        ErrorLine("ELF header mismatch!");
        return EFI_LOAD_ERROR;
    }

    Elf64_Phdr *phdrs = (Elf64_Phdr *)((uint8_t *)elf_data + ehdr->e_phoff);
    out->kernel_page_count = 0;
    for (int i = 0; i < ehdr->e_phnum; ++i)
    {
        Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD)
            continue;

        out->kernel_page_count += EFI_SIZE_TO_PAGES(ph->p_memsz);
    }

    out->kernel_virtual_base = (virtual_address_t)phdrs[0].p_vaddr;
    return EFI_SUCCESS;
}

EFI_STATUS get_memmap(EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi, UINTN *mapKey)
{
    EFI_STATUS status;

    UINTN mapSize = 0, descSize;
    UINT32 descVersion;
    EFI_MEMORY_DESCRIPTOR *memMap = NULL;

    TRYWRAP((SystemTable->BootServices->GetMemoryMap, 5,
             &mapSize, memMap, mapKey, &descSize, &descVersion));

    // Allocate space
    mapSize += descSize * 8; // Safety margin
    TRYWRAP((SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, mapSize, (void **)&memMap));

    // Now get the actual map
    TRYWRAPS((SystemTable->BootServices->GetMemoryMap, 5,
              &mapSize, memMap, mapKey, &descSize, &descVersion),
             "Could not get memory map");

    bi->memory_map.memory_map = memMap;
    bi->memory_map.memory_map_size = mapSize;
    bi->memory_map.descriptor_size = descSize;
    bi->memory_map.descriptor_version = descVersion;

    return EFI_SUCCESS;
}

EFI_STATUS map_page(page_virtual_address_t virt_addr, page_physical_address_t phys_addr, PageAttributes attrs, page_table_physical_ptr_t pageTable)
{
    EFI_STATUS status;
    page_physical_address_t page_addr;
    page_table_entry_t page_entry;
    page_table_entry_t *entries = *pageTable;

    // Recurse page tables and make sure they exist; find the final page table
    for (int level = 4; level >= 2; level--)
    {
        int idx = PT_IDX(virt_addr, level);
        page_entry = entries[idx];
        page_addr = page_entry & PAGE_ADDR_MASK;
        if ((page_entry & PAGE_PRESENT) == 0)
        {
            // Create new page table page
            TRYWRAP((BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
                     1, (EFI_PHYSICAL_ADDRESS *)&page_addr));
            // Clear the new page
            memset((EFI_PHYSICAL_ADDRESS *)&page_addr, 4096, 0);

            page_entry = page_addr & PAGE_ADDR_MASK | PAGE_PRESENT | PAGE_RW | PAGE_NX;
            entries[idx] = page_entry;
        }
        entries = *(page_table_physical_ptr_t)(page_addr);
    }

    // Set the new entry in the lowest page table
    entries[PT_L1_IDX(virt_addr)] = phys_addr & PAGE_ADDR_MASK | attrs;

    return EFI_SUCCESS;
}

EFI_STATUS map_pages(page_virtual_address_t virt_addr, page_physical_address_t phys_addr, PageAttributes attr, int pages, page_table_physical_ptr_t pageTable)
{
    EFI_STATUS status;
    while (pages--)
    {
        TRYWRAPFN(map_page(virt_addr, phys_addr, attr, pageTable));
        virt_addr += EFI_PAGE_SIZE;
        phys_addr += EFI_PAGE_SIZE;
    }

    return EFI_SUCCESS;
}

EFI_STATUS map_new_pages(page_virtual_address_t virt_addr, page_physical_address_ptr_t phys_addr_out, PageAttributes attr, int pages, page_table_physical_ptr_t pageTable)
{
    EFI_STATUS status;
    TRYWRAP((BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
             pages, (EFI_PHYSICAL_ADDRESS *)phys_addr_out));

    TRYWRAPFN(map_pages(virt_addr, *phys_addr_out, attr, pages, pageTable));

    return EFI_SUCCESS;
}

EFI_STATUS create_page_tables(page_table_physical_address_ptr_t page_table_out)
{
    EFI_STATUS status;

    TraceLine("Allocating top level page table...");
    TRYWRAP((BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
             1, (EFI_PHYSICAL_ADDRESS *)page_table_out));

    TraceLine("New page table physical address: %p", *page_table_out);

    page_table_physical_ptr_t pageTable = (page_table_physical_ptr_t)*page_table_out;

    // Clear the new page
    memset(pageTable, sizeof(page_table_t), 0);

    // Map it in(to itself)
    TRYWRAPFN(map_page(PT_L4_BASE, (page_physical_address_t)pageTable, PAGE_RW | PAGE_NX, pageTable));

    return EFI_SUCCESS;
}

EFI_STATUS get_mp_info(EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi, UINTN *cpuCount)
{
    EFI_STATUS status;
    EFI_MP_SERVICES_PROTOCOL *MpServices;
    UINTN enabledCount;

    TRYWRAPS((BS->LocateProtocol, 3, &gEfiMpServiceProtocolGuid, NULL, (void **)&MpServices),
             "Failed to locate processor protocol");

    TRYWRAP((MpServices->GetNumberOfProcessors, 3, MpServices, cpuCount, &enabledCount));

    bi->cpu_count = *cpuCount;

    return EFI_SUCCESS;
}

EFI_GUID gEfiMpServiceProtocolGuid = EFI_MP_SERVICES_PROTOCOL_GUID;