#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void halt_cpu();
    void enable_interrupts();
    void disable_interrupts();

#ifdef __cplusplus
} // extern "C"
#endif