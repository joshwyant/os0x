// kernel/arch/amd64/start.S
.intel_syntax noprefix
.global _start

_start:
    call kernel_boot_uefi
    h: hlt
    jmp h
    