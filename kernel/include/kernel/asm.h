#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void halt_cpu();
    void enable_interrupts();
    void disable_interrupts();
    void invalidate_page(uintptr_t addr);

#ifdef __cplusplus
} // extern "C"
#endif