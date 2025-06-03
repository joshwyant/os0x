OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kernel_boot)

SECTIONS
{
  /* Load kernel at 1MB */
  . = 1M;

  /* Text/code section */
  .text : {
    *(.text .text.*)
  }

  /* Read-only data */
  .rodata : {
    *(.rodata .rodata.*)
  }

  /* Initialized data */
  .data : {
    *(.data .data.*)
  }

  /* BSS (zero-initialized data) */
  .bss : {
    *(COMMON)
    *(.bss .bss.*)
  }

  /* Ensure page alignment for stack or other uses if needed */
  /DISCARD/ : {
    *(.eh_frame .note .comment)
  }
}
