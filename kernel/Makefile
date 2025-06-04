.PHONY: all clean rebuild

ARCH := amd64
INCLUDES := -I../include
EFI_PREFIX ?= /usr/local
EFI_INCLUDES := -I../boot/include -I$(EFI_PREFIX)/include/efi -I$(EFI_PREFIX)/include/efi/x86_64
KERNEL_TARGET := x86_64-unknown-elf
KERNEL_AS := x86_64-linux-gnu-gcc
KERNEL_LD := x86_64-linux-gnu-ld
KERNEL_CC := clang
KERNEL_SRC := .
KERNEL_OBJ := $(KERNEL_SRC)/obj
KERNEL_OBJS := $(KERNEL_OBJ)/uefi.o $(KERNEL_OBJ)/main.o $(KERNEL_OBJ)/asm.o $(KERNEL_OBJ)/lib/string.o
KERNEL_BIN := $(KERNEL_SRC)/bin
KERNEL_DEFINES := 
KERNEL_CFLAGS := -target $(KERNEL_TARGET) -ffreestanding -nostdlib -fno-builtin -fno-pic -c -MMD -MP
KERNEL_ASFLAGS := -ffreestanding -m64 -c -MMD -MP
KERNEL_INCLUDE := $(INCLUDES) $(EFI_INCLUDES) -I$(KERNEL_SRC)/include
KERNEL_CC_INVOKE := $(KERNEL_CC) $(KERNEL_CFLAGS) $(KERNEL_DEFINES) $(KERNEL_INCLUDE)

all: $(KERNEL_BIN)/kernel.elf

rebuild: clean all

$(KERNEL_OBJ):
	@mkdir -p $@ $@/lib

$(KERNEL_BIN):
	@mkdir -p $@

$(KERNEL_OBJ)/%.o: $(KERNEL_SRC)/arch/$(ARCH)/%.S | $(KERNEL_OBJ)
	$(KERNEL_AS) $(KERNEL_ASFLAGS) -o $@ $<

$(KERNEL_OBJ)/%.o: $(KERNEL_SRC)/%.c | $(KERNEL_OBJ)
	$(KERNEL_CC_INVOKE) $< -o $@

$(KERNEL_BIN)/kernel.elf: $(KERNEL_OBJS) | $(KERNEL_BIN)
	$(KERNEL_LD) -nostdlib -z max-page-size=0x1000 -z noexecstack \
		-T $(KERNEL_SRC)/link.ld -o $@ $(KERNEL_OBJ)/**/*.o

clean:
	$(RM) -r $(KERNEL_OBJ)
	$(RM) -r $(KERNEL_BIN)

-include $(wildcard $(KERNEL_OBJ)/*.d)