// kernel/arch/amd64/start.S
.intel_syntax noprefix
.global _start

_start:
    jmp kernel_boot_uefi
    