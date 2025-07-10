#pragma once

#include "asm.h"
#include "kernel/kernel.h"

static inline void freeze() {
  disable_interrupts();
  while (true)
    halt_cpu();
}