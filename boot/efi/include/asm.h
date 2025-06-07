#pragma once

#include <stdint.h>
#include "paging.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void enable_nxe();
    void disable_interrupts();
    void trampoline(page_table_physical_address_t cr3, virtual_address_t stack, physical_address_t boot_info, virtual_address_t entry);

#ifdef __cplusplus
} // extern "C"
#endif