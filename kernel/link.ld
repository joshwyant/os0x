OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kernel_boot)

. = 0xFFFFFF0000000000;

SECTIONS
{
  KERNEL_VMA = .;

  .text : ALIGN(0x1000)
  {
    *(.text .text.*)
  }

  .rodata : ALIGN(0x1000)
  {
    *(.rodata .rodata.*)
  }

  .data : ALIGN(0x1000)
  {
    *(.data .data.*)
  }

  .bss : ALIGN(0x1000)
  {
    *(COMMON)
    *(.bss .bss.*)
  }

  /* Ensure page alignment for stack or other uses if needed */
  /DISCARD/ : {
    *(.eh_frame .note .comment)
  }

  . = ALIGN(0x1000);
  _kernel_end = .;
}
