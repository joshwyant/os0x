// boot/efi/arch/amd64/asm.S
.intel_syntax noprefix
.global trampoline
trampoline:
    cli

    mov rax, rdi           // New cr3 in rdi
    
    mov rsp, rsi           // New stack in rsi
    mov rdi, rdx           // Boot info in rdx

    freeze:
    hlt
    jmp freeze

    mov cr3, rax           // Switch to new page table

    jmp rcx                // Kernel entry in rcx

.global enable_nxe
enable_nxe:
    mov ecx, 0xC0000080    // EFER MSR
    rdmsr
    bts eax, 11            // Set NXE (bit 11) (No execute enable)
    wrmsr
    ret

.global disable_interrupts
disable_interrupts:
    cli
    ret