#pragma once

#include <stddef.h>
#include <stdint.h>

#include "kernel/asm.h"
#include "kernel/ktypes.h"
#include "kernel/paging.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void kernel_main();

#ifdef __cplusplus
} // extern "C"
namespace k
{
    class KernelContainer
    {
    public:
        // paging.h
        virtual KernelMemoryLayout &kernel_memory_layout_instance() = 0;
        virtual PhysicalMemoryAllocator &physical_memory_allocator_instance() = 0;
        virtual VirtualMemoryMapper &virtual_memory_mapper_instance() = 0;

    protected:
        KernelContainer() {};
    };
}
#endif

static void
freeze()
{
    disable_interrupts();
    while (TRUE)
        halt_cpu();
}