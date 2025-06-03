#pragma once

#include <stddef.h>
#include <stdint.h>

#include "asm.h"
#include "ktypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void kernel_main();

#ifdef _cplusplus
} // extern "C"
#endif

static void freeze()
{
    disable_interrupts();
    while (TRUE)
        halt_cpu();
}