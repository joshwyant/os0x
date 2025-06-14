.PHONY: all clean rebuild

ARCH := amd64
INCLUDES := -I../include
EFI_PREFIX ?= /usr/local
EFI_INCLUDES := -I../boot/include -I$(EFI_PREFIX)/include/efi -I$(EFI_PREFIX)/include/efi/x86_64
KERNEL_TARGET := x86_64-unknown-elf
KERNEL_AS := x86_64-linux-gnu-gcc
KERNEL_LD := x86_64-linux-gnu-ld
KERNEL_CC := clang
KERNEL_OBJS := obj/uefi.o obj/main.o obj/asm.o obj/lib/c/string.o
KERNEL_DEFINES := 
KERNEL_CFLAGS := -target $(KERNEL_TARGET) -ffreestanding -nostdlib -fno-builtin -fno-pic -c -MMD -MP
KERNEL_ASFLAGS := -ffreestanding -m64 -c -MMD -MP
KERNEL_INCLUDE := $(INCLUDES) $(EFI_INCLUDES) -Iinclude
KERNEL_CC_INVOKE := $(KERNEL_CC) $(KERNEL_CFLAGS) $(KERNEL_DEFINES) $(KERNEL_INCLUDE)

all: bin/kernel.elf

rebuild: clean all

obj:
	@mkdir -p $@ $@/lib $@/lib/c

bin:
	@mkdir -p $@

obj/%.o: arch/$(ARCH)/%.S | obj
	$(KERNEL_AS) $(KERNEL_ASFLAGS) -o $@ $<

obj/%.o: %.c | obj
	$(KERNEL_CC_INVOKE) $< -o $@

bin/kernel.elf: $(KERNEL_OBJS) | bin
	$(KERNEL_LD) -nostdlib -z max-page-size=0x1000 -z noexecstack \
		-T link.ld -o $@ obj/*.o
#obj/**/*.o

clean:
	$(RM) -r obj
	$(RM) -r bin

-include $(wildcard obj/*.d)