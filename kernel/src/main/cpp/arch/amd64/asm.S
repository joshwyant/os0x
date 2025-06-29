// kernel/arch/amd64/asm.S
.intel_syntax noprefix
.global halt_cpu
.global enable_interrupts
.global disable_interrupts
.global invalidate_page

// Halts the CPU until the next interrupt
halt_cpu:
    hlt
    ret

// Enables interrupts (sets IF flag)
enable_interrupts:
    sti
    ret

// Disables interrupts (clears IF flag)
disable_interrupts:
    cli
    ret

invalidate_page:
    invlpg [rdi]
    ret