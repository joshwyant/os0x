#pragma once

#include "asm.h"
#include "kernel/kernel.h"

static void freeze() {
  disable_interrupts();
  while (true)
    halt_cpu();
}