#include "main.h"

EFI_STATUS load_kernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, const void *kernel, size_t kernel_size, boot_info_t *bi)
{
    EFI_STATUS status;
    kernel_image_t kernel_info;
    page_table_physical_ptr_t pageTable;

    TraceLine("Creating page tables...");
    TRYWRAPFNS(create_page_tables((page_table_physical_address_t *)&pageTable),
               "Failed to create page tables");

    bi->page_table_physical = (page_table_physical_address_t)pageTable;
    bi->page_table_virtual = PT_L4_BASE;

    virtual_address_t stack_pointer;
    TraceLine("Mapping virtual address space...");
    TRYWRAPFNS(map_virtual_address_space(SystemTable, kernel, kernel_size, &kernel_info, bi, &stack_pointer, pageTable),
               "Failed to map virtual address space");

    // TraceLine("Mapping the kernel into virtual memory...");
    // TRYWRAPFNS(map_kernel(kernel, kernel_size, &kernel_info, pageTable),
    //            "Failed to map the kernel into virtual memory");

    //__asm__ volatile("mov %0, %%cr3" ::"r"(pageTable) : "memory");

    TraceLine("Listing PML4 entries:");

    for (int i = 0; i < PAGE_TABLE_ENTRY_COUNT; i++)
    {
        TraceLine("Entry %d: %llp", i, (*pageTable)[i]);
        if (i < (PAGE_TABLE_ENTRY_COUNT - 1) && (*pageTable)[i] == 0 && (*pageTable)[i + 1] == 0)
        {
            TraceLine("...");
            while (i < (PAGE_TABLE_ENTRY_COUNT - 2) && (*pageTable)[i + 2] == 0)
                i++;
            continue;
        }
    }
    TraceLine("Address of trampoline %llp", trampoline);

    TraceLine("Enable NXE...");
    enable_nxe();

    UINTN mapKey;
    TraceLine("Getting system memory map and calling the kernel...");
    TRYWRAPFNS(get_memmap(SystemTable, &bi->memory_map, &mapKey),
               "Failed to get memory map");

    // No printing or allocation after getting the memory map
    // TRYWRAPFN(check_addr("memory map", (virtual_address_t)bi->memory_map, (page_table_entry_physical_ptr_t)pageTable));

    // TraceLine("Calling kernel entry point...");
    // No printing or allocation after getting the memory map
    TRYWRAPFNS(enter_kernel(ImageHandle, SystemTable, &kernel_info, stack_pointer, (page_table_physical_address_t)pageTable, bi, mapKey),
               "Failed to call kernel entry point");

    return EFI_SUCCESS;
}

EFI_STATUS enter_kernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, kernel_image_t *kernel_info, virtual_address_t stack_pointer, page_table_physical_address_t page_table, boot_info_t *bi, UINTN mapKey)
{
    EFI_STATUS status;

    // TraceLine("Validating kernel entry mapping: %llp", kernel_info->entry);
    // page_table_entry_t *lvl4 = ((page_table_entry_t *)page_table);
    // int l4 = PT_IDX(kernel_info->entry, 4);
    // int l3 = PT_IDX(kernel_info->entry, 3);
    // int l2 = PT_IDX(kernel_info->entry, 2);
    // int l1 = PT_IDX(kernel_info->entry, 1);
    // TraceLine("L4[%d]: %llp", l4, lvl4[l4]);

    // TraceLine("trampoline(page_table: %llp, stack_pointer: %llp, boot_info: %llp, entry: %llp)", page_table, stack_pointer, bi, kernel_info->entry);
    // while (1)
    //     ;

    // DebugLine("Exiting boot services...");
    // No printing or allocation after getting the memory map
    TRYWRAPS(((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mapKey),
             "Could not exit boot services");

    // DebugLine("Kernel loaded. Executing...");
    // No printing after exiting boot services
    trampoline(page_table, stack_pointer, (physical_address_t)bi, (virtual_address_t)kernel_info->entry);

    // Should kernel return
    return EFI_SUCCESS;
}

EFI_STATUS check_addr(const char *name, virtual_address_t vaddr, page_table_entry_physical_ptr_t pageTable)
{
    page_table_entry_t l3 = pageTable[PT_L4_IDX(vaddr)];
    pageTable = (page_table_entry_physical_ptr_t)(l3 & PAGE_ADDR_MASK);

    page_table_entry_t l2 = pageTable[PT_L3_IDX(vaddr)];
    pageTable = (page_table_entry_physical_ptr_t)(l2 & PAGE_ADDR_MASK);

    page_table_entry_t l1 = pageTable[PT_L2_IDX(vaddr)];
    pageTable = (page_table_entry_physical_ptr_t)(l1 & PAGE_ADDR_MASK);

    page_table_entry_t entry = pageTable[PT_L1_IDX(vaddr)];
    physical_address_t paddr = (physical_address_t)(entry & PAGE_ADDR_MASK);

    if (paddr == 0)
    {
        TraceLine("Entry for %a (%llp) doesn't exist!", name, vaddr);
        return EFI_LOAD_ERROR;
    }
    else
    {
        TraceLine("Entry for %a (%llp): %llp", name, vaddr, entry);
    }

    const unsigned char *bytes = (const unsigned char *)paddr;
    TraceLine("  First bytes: 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);

    return EFI_SUCCESS;
}

EFI_STATUS map_virtual_address_space(EFI_SYSTEM_TABLE *SystemTable, const void *kernel, size_t kernel_size, kernel_image_t *kernel_info, boot_info_t *bi, virtual_address_ptr_t stack_pointer_out, page_table_physical_ptr_t pageTable)
{
    EFI_STATUS status;
    UINTN cpuCount;
    page_table_entry_physical_ptr_t pml4 = (page_table_entry_physical_ptr_t)pageTable;

    // Map in the kernel
    TraceLine("Mapping in the kernel...");
    page_virtual_address_t first_page, next_page;
    map_kernel(kernel, kernel_size, kernel_info, &first_page, &next_page, pageTable);

    TRYWRAPFN(check_addr("kernel", (virtual_address_t)first_page, pml4));
    TRYWRAPFN(check_addr("kernel entry", (virtual_address_t)kernel_info->entry, pml4));

    // Leave some scratch space for manipulating page tables later
    // 1 page for a new PT
    // 1 page for a new PD
    // 1 page for a new PDP
    bi->page_table_scratch_addr = next_page;
    next_page += 3 * EFI_PAGE_SIZE;

    // for (;;)
    //     ;

    // Map in the stacks
    page_virtual_address_t stack_page = next_page;
    TRYWRAPFNS(get_mp_info(SystemTable, bi, &cpuCount),
               "Failed to get CPU count");
    TraceLine("Mapping in %dkb stacks for %d CPUs...", STACK_SIZE >> 10, cpuCount);
    *stack_pointer_out = (virtual_address_t)(next_page + EFI_PAGE_SIZE // guard page
                                             + STACK_SIZE);            // Top of the stack
    TraceLine("Stack pointer: %llp", *stack_pointer_out);

    int stack_pages = EFI_SIZE_TO_PAGES(STACK_SIZE);
    for (int i = 0; i < cpuCount; i++)
    {
        TraceLine("Stack %d ========", i + 1);
        TraceLine("(Guard page: %llp)", next_page);
        next_page += EFI_PAGE_SIZE; // guard page
        TraceLine("Allocating %d pages for stack at %llp", stack_pages, next_page);
        page_physical_address_t stack_addr;
        TRYWRAPFN(map_new_pages(next_page, &stack_addr, PageAttributes::PAGE_PRESENT | PageAttributes::PAGE_RW | PageAttributes::PAGE_NX, stack_pages, pageTable));
        TraceLine("Physical address of stack: %llp", stack_addr);

        // Zero it out
        memset((void *)stack_addr, 0, STACK_SIZE);
        next_page += STACK_SIZE;
    }

    TRYWRAPFN(check_addr("(ss:esp - 8)", *stack_pointer_out - 8, pml4));

    // for (;;)
    //     ;

    // Map in the frame buffer
    page_virtual_address_t framebuf_page = next_page;
    int framebuf_pages = EFI_SIZE_TO_PAGES(bi->graphics_info.framebuffer_size);
    bi->graphics_info.framebuffer_virtual_base = (uint32_t *)next_page;
    TraceLine("Mapping in %d frame buffer pages from %llp (phys) to %llp (virt)...", framebuf_pages, bi->graphics_info.framebuffer_base, bi->graphics_info.framebuffer_virtual_base);
    // Suppress output
    enum BootLogLevel prevLevel = logLevel;
    if (logLevel > DebugLevel)
        logLevel = DebugLevel;
    TRYWRAPFN(map_pages((page_virtual_address_t)bi->graphics_info.framebuffer_virtual_base,
                        (page_physical_address_t)bi->graphics_info.framebuffer_base,
                        PageAttributes::PAGE_PRESENT | PageAttributes::PAGE_RW | PageAttributes::PAGE_NX,
                        framebuf_pages, pageTable));
    next_page += framebuf_pages * EFI_PAGE_SIZE;
    logLevel = prevLevel;
    TraceLine("Mapped in %d frame buffer pages from %llp (phys) to %llp (virt).", framebuf_pages, bi->graphics_info.framebuffer_base, bi->graphics_info.framebuffer_virtual_base);

    TRYWRAPFN(check_addr("frame buffer", framebuf_page, pml4));

    // for (;;)
    //     ;

    // Map in initrd image
    page_virtual_address_t initrd_page = next_page;
    int initrd_pages = EFI_SIZE_TO_PAGES(bi->initrd_size);
    TraceLine("Mapping in initrd.img, %d pages at %llp", initrd_pages, next_page);
    TRYWRAPFN(map_pages(next_page, (page_physical_address_t)bi->initrd_base, PageAttributes::PAGE_PRESENT | PageAttributes::PAGE_RW | PageAttributes::PAGE_NX, initrd_pages, pageTable));
    bi->initrd_base = (uint32_t *)next_page; // physical to virtual
    next_page += initrd_pages * EFI_PAGE_SIZE;

    TRYWRAPFN(check_addr("initrd", initrd_page, pml4));

    // for (;;)
    //     ;

    // Identity map in our loader code and data
    boot_memmap_t initmm;
    UINTN mapKey;
    page_virtual_address_t loader_page = (uint64_t)-1, loader_code_page;
    TraceLine("Getting temporary memory map...");
    TRYWRAPFNS(get_memmap(SystemTable, &initmm, &mapKey),
               "Failed to get memory map");
    uint8_t *desc = (uint8_t *)initmm.memory_map;
    for (UINTN i = 0; i < initmm.memory_map_size; i += initmm.descriptor_size)
    {
        EFI_MEMORY_DESCRIPTOR *d = (EFI_MEMORY_DESCRIPTOR *)(desc + i);

        switch (d->Type)
        {
        case EfiLoaderCode:
            loader_code_page = d->PhysicalStart;
            TraceLine("Mapping in loader code section %llp with %d pages...", d->PhysicalStart, d->NumberOfPages);
            TRYWRAPFN(map_pages(d->PhysicalStart, d->PhysicalStart, PageAttributes::PAGE_PRESENT, d->NumberOfPages, pageTable));
            break;
        case EfiLoaderData:
            if (d->PhysicalStart < loader_page)
                loader_page = d->PhysicalStart;
            TraceLine("Mapping in loader data section %llp with %d pages...", d->PhysicalStart, d->NumberOfPages);
            TRYWRAPFN(map_pages(d->PhysicalStart, d->PhysicalStart, PageAttributes::PAGE_PRESENT | PageAttributes::PAGE_RW | PageAttributes::PAGE_NX, d->NumberOfPages, pageTable));
            break;
#if 1
        case EfiReservedMemoryType:
            TraceLine("EfiReservedMemoryType        %08llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiBootServicesCode:
            TraceLine("EfiBootServicesCode          %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiBootServicesData:
            TraceLine("EfiBootServicesData          %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiRuntimeServicesCode:
            TraceLine("EfiRuntimeServicesCode       %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiRuntimeServicesData:
            TraceLine("EfiRuntimeServicesData       %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiConventionalMemory:
            TraceLine("EfiConventionalMemory        %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiUnusableMemory:
            TraceLine("EfiUnusableMemory            %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiACPIReclaimMemory:
            TraceLine("EfiACPIReclaimMemory         %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiACPIMemoryNVS:
            TraceLine("EfiACPIMemoryNVS             %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiMemoryMappedIO:
            TraceLine("EfiMemoryMappedIO            %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiMemoryMappedIOPortSpace:
            TraceLine("EfiMemoryMappedIOPortSpace   %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiPalCode:
            TraceLine("EfiPalCode                   %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiPersistentMemory:
            TraceLine("EfiPersistentMemory          %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiUnacceptedMemoryType:
            TraceLine("EfiUnacceptedMemoryType      %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
        case EfiMaxMemoryType:
            TraceLine("EfiMaxMemoryType             %llp-%llp", d->PhysicalStart, d->PhysicalStart + d->NumberOfPages * EFI_PAGE_SIZE - 1);
            break;
#endif
        }
    }
    TraceLine("Freeing temporary memory map...");
    TRYWRAPS(((void *)BS->FreePool, 1, initmm.memory_map), "Failed to free temporary memory map");

    // for (;;)
    //     ;

    TraceLine("End of mapping: %llp", next_page);

    TRYWRAPFN(check_addr("kernel", first_page, pml4));
    TRYWRAPFN(check_addr("kernel entry", (virtual_address_t)kernel_info->entry, pml4));
    TRYWRAPFN(check_addr("(ss:esp - 8)", *stack_pointer_out - 8, pml4));
    TRYWRAPFN(check_addr("frame buffer", framebuf_page, pml4));
    TRYWRAPFN(check_addr("initrd", initrd_page, pml4));
    TRYWRAPFN(check_addr("loader data", loader_page, pml4));
    TRYWRAPFN(check_addr("loader code", loader_code_page, pml4));
    TRYWRAPFN(check_addr("boot_info", (virtual_address_t)bi, pml4));

    // for (;;)
    //     ;

    return EFI_SUCCESS;
}

EFI_STATUS map_kernel(const void *elf_data, size_t elf_size, kernel_image_t *out, page_virtual_address_ptr_t first_page, page_virtual_address_ptr_t next_page, page_table_physical_ptr_t pageTable)
{
    EFI_STATUS status;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_data;
    uint8_t elfmag[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
    if (_memcmp(ehdr->e_ident, elfmag, sizeof(elfmag)))
        return EFI_LOAD_ERROR;

    Elf64_Phdr *phdrs = (Elf64_Phdr *)((uint8_t *)elf_data + ehdr->e_phoff);
    out->kernel_page_count = 0;
    out->kernel_virtual_base = (virtual_address_t)phdrs[0].p_vaddr;
    out->entry = (kernel_entry_t)ehdr->e_entry;
    out->kernel_code_pages = EFI_SIZE_TO_PAGES(phdrs[0].p_memsz);
    *first_page = out->kernel_virtual_base;
    *next_page = *first_page;

    TraceLine("  Kernel base:  %llp", out->kernel_virtual_base);
    TraceLine("  Kernel entry: %llp", out->entry);
    TraceLine("  Kernel code pages: %d", out->kernel_code_pages);

    for (int i = 0; i < ehdr->e_phnum; ++i)
    {
        Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD)
            continue;

        TraceLine("  Section       %llp:", ph->p_vaddr);
        TraceLine("  Allocating to %llp", *next_page);

        UINTN pages = EFI_SIZE_TO_PAGES(ph->p_memsz);

        page_physical_address_t physaddr;
        PageAttributes attr = PageAttributes::PAGE_PRESENT;
        if (!(ph->p_flags & PF_X))
        {
            TraceLine("    No execute");
            attr |= PageAttributes::PAGE_NX;
        }
        if ((ph->p_flags) & PF_W)
        {
            attr |= PageAttributes::PAGE_RW;
            TraceLine("    Writeable");
        }
        TRYWRAPFN(map_new_pages(*next_page, &physaddr, attr, pages, pageTable));
        TraceLine("    Allocated to physical address @ %d pages: %llp", pages, physaddr);

        memcpy((void *)physaddr,
               (uint8_t *)elf_data + ph->p_offset,
               ph->p_filesz);
        TraceLine("    Copied 0x%x bytes", ph->p_filesz);

        if (ph->p_memsz > ph->p_filesz)
            memset((uint8_t *)physaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
        TraceLine("    Memsize is 0x%x bytes, clearing 0x%x additional bytes", ph->p_memsz, ph->p_memsz - ph->p_filesz);

        out->kernel_page_count += pages;
        *next_page += pages * EFI_PAGE_SIZE;
    }

    return EFI_SUCCESS;
}

EFI_STATUS get_memmap(EFI_SYSTEM_TABLE *SystemTable, boot_memmap_t *mm, UINTN *mapKey)
{
    EFI_STATUS status;

    UINTN mapSize = 0, descSize;
    UINT32 descVersion;
    EFI_MEMORY_DESCRIPTOR *memMap = NULL;

    // Passing in 0 and NULL gives us the minimum map size and an error
    uefi_call_wrapper((void *)SystemTable->BootServices->GetMemoryMap, 5,
                      &mapSize, memMap, mapKey, &descSize, &descVersion);
    TraceLine("Memory map size: 0x%x, descriptor size: 0x%x, version: %d", mapSize, descSize, descVersion);

    // Allocate space
    mapSize += descSize * 8; // Safety margin
    TRYWRAP(((void *)SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, mapSize, (void **)&memMap));

    // Now get the actual map
    TRYWRAPS(((void *)SystemTable->BootServices->GetMemoryMap, 5,
              &mapSize, memMap, mapKey, &descSize, &descVersion),
             "Could not get memory map");

    // No more allocation or printing allowed if using map key to exit boot services

    mm->memory_map = memMap;
    mm->memory_map_size = mapSize;
    mm->descriptor_size = descSize;
    mm->descriptor_version = descVersion;

    return EFI_SUCCESS;
}

EFI_STATUS map_page(page_virtual_address_t virt_addr, page_physical_address_t phys_addr, PageAttributes attrs, page_table_physical_ptr_t pageTable)
{
    EFI_STATUS status;
    page_table_entry_t page_entry;
    page_table_entry_physical_ptr_t entries = (page_table_entry_physical_ptr_t)*pageTable;
    int idx;

    TraceLine("Mapping page for virtual address %llp...", virt_addr);

    // Recurse page tables and make sure they exist; find the final page table
    for (int level = 4; level >= 2; level--)
    {
        idx = PT_IDX(virt_addr, level);
        TraceLine("  Page table level %ld @ %llp, index 0x%x", level, entries, idx);
        page_entry = entries[idx];
        // TraceLine("    Current entry: %llp", page_entry);
        if ((page_entry & static_cast<uint64_t>(PageAttributes::PAGE_PRESENT)) == 0)
        {
            page_physical_address_t page_addr;
            // Create new page table page
            TRYWRAP(((void *)BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
                     1, (EFI_PHYSICAL_ADDRESS *)&page_addr));
            page_physical_ptr_t page = (page_physical_ptr_t)page_addr;
            // Clear the new page
            memset((void *)page_addr, 0, EFI_PAGE_SIZE);

            page_entry = page_addr & PAGE_ADDR_MASK | static_cast<uint64_t>(PageAttributes::PAGE_PRESENT | PageAttributes::PAGE_RW) /*| PageAttributes::PAGE_NX*/;

            TraceLine("    Allocated physical page %llp, entry %llp", page_addr, page_entry);
            entries[idx] = page_entry;
        }
        entries = (page_table_entry_physical_ptr_t)(page_table_physical_ptr_t)(page_entry & PAGE_ADDR_MASK);
        // TraceLine("    Next level: %llp", entries);
    }

    // Set the new entry in the lowest page table
    idx = PT_L1_IDX(virt_addr);
    TraceLine("  Page table level 1, index 0x%x", idx);
    page_entry = phys_addr & PAGE_ADDR_MASK | static_cast<unsigned long long>(attrs);
    entries[idx] = page_entry;
    TraceLine("    New entry: %llp%a%a%a", page_entry, ((attrs & PageAttributes::PAGE_PRESENT) == PageAttributes::PAGE_PRESENT) ? " P" : "", ((attrs & PageAttributes::PAGE_RW) == PageAttributes::PAGE_RW) ? " W" : "", ((attrs & PageAttributes::PAGE_NX) == PageAttributes::PAGE_NX) ? " NX" : "");
    const unsigned char *bytes = (const unsigned char *)phys_addr;
    TraceLine("    First bytes: 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);

    return EFI_SUCCESS;
}

EFI_STATUS map_pages(page_virtual_address_t virt_addr, page_physical_address_t phys_addr, PageAttributes attr, int pages, page_table_physical_ptr_t pageTable)
{
    EFI_STATUS status;
    enum BootLogLevel prevLevel = logLevel;
    for (int i = 0; i < pages; i++)
    {
        if (i == 1 && logLevel > WarningLevel && pages > 2)
        {
            TraceLine("...");
            logLevel = DebugLevel;
        }
        if (i == pages - 1)
            logLevel = prevLevel;
        TraceLine("Page %d of %d...", i + 1, pages);
        TRYWRAPFN(map_page(virt_addr, phys_addr, attr, pageTable));
        virt_addr += EFI_PAGE_SIZE;
        phys_addr += EFI_PAGE_SIZE;
    }

    return EFI_SUCCESS;
}

EFI_STATUS map_new_pages(page_virtual_address_t virt_addr, page_physical_address_ptr_t phys_addr_out, PageAttributes attr, int pages, page_table_physical_ptr_t pageTable)
{
    TraceLine("Creating %d new pages...", pages);
    EFI_STATUS status;
    TRYWRAP(((void *)BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
             pages, (EFI_PHYSICAL_ADDRESS *)phys_addr_out));
    TraceLine("Created %d new pages at %llp. Mapping them in...", pages, *phys_addr_out);

    TRYWRAPFN(map_pages(virt_addr, *phys_addr_out, attr, pages, pageTable));

    return EFI_SUCCESS;
}

EFI_STATUS create_page_tables(page_table_physical_address_ptr_t page_table_out)
{
    EFI_STATUS status;

    TraceLine("Allocating top level page table...");
    TRYWRAP(((void *)BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
             1, (EFI_PHYSICAL_ADDRESS *)page_table_out));

    TraceLine("New page table physical address: %llp", *page_table_out);

    page_table_physical_ptr_t pageTable = (page_table_physical_ptr_t)*page_table_out;

    // Clear the new page
    memset(pageTable, 0, sizeof(page_table_t));

    TraceLine("Cleared the page table.");

    // Map it in(to itself)
    TraceLine("Mapping the page table into itself...");
    // This is redundant if we could just set the entry manually
    TRYWRAPFN(map_page(PT_L4_BASE, (page_physical_address_t)pageTable, PageAttributes::PAGE_PRESENT | PageAttributes::PAGE_RW /*| PageAttributes::PAGE_NX*/, pageTable));
    TraceLine("Page tables created.");

    return EFI_SUCCESS;
}

EFI_STATUS get_mp_info(EFI_SYSTEM_TABLE *SystemTable, boot_info_t *bi, UINTN *cpuCount)
{
    EFI_STATUS status;
    EFI_MP_SERVICES_PROTOCOL *MpServices;
    UINTN enabledCount;

    TRYWRAPS(((void *)BS->LocateProtocol, 3, &gEfiMpServiceProtocolGuid, NULL, (void **)&MpServices),
             "Failed to locate processor protocol");

    TRYWRAP(((void *)MpServices->GetNumberOfProcessors, 3, MpServices, cpuCount, &enabledCount));

    bi->cpu_count = *cpuCount;

    return EFI_SUCCESS;
}

EFI_GUID gEfiMpServiceProtocolGuid = EFI_MP_SERVICES_PROTOCOL_GUID;