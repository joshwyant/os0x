.PHONY: all clean rebuild

ARCH := amd64
PROJ_ROOT_DIR := ../../../..
PACKAGES_SRC := $(PROJ_ROOT_DIR)/packages
EFI_SHIM_SRC := $(PACKAGES_SRC)/efi_shim/src/main/cpp
INCLUDES := -I$(PROJ_ROOT_DIR)/include
EFI_PREFIX ?= /usr/local
EFI_INCLUDES := -I$(EFI_PREFIX)/include/efi -I$(EFI_PREFIX)/include/efi/x86_64
KERNEL_TARGET := x86_64-unknown-elf
KERNEL_AS := x86_64-linux-gnu-gcc
KERNEL_LD := x86_64-linux-gnu-ld
KERNEL_CC := clang
KERNEL_OBJS := 	obj/start.o \
				obj/asm.o \
				obj/main.o \
				obj/paging.o \
				obj/init/uefi.o \
				obj/init/init.o \
				obj/lib/c/runtime_support.o \
				obj/lib/cpp/runtime_support.o
KERNEL_DEFINES := 
KERNEL_CFLAGS := -target $(KERNEL_TARGET) -ffreestanding -fno-exceptions -fno-rtti -nostdlib -fno-builtin -fno-pic -mcmodel=large -c -MMD -MP
KERNEL_ASFLAGS := -ffreestanding -m64 -c -MMD -MP
KERNEL_ROOT_DIR := ../../..
KERNEL_INCLUDE := $(INCLUDES) $(EFI_INCLUDES) -I$(KERNEL_ROOT_DIR)/include -I$(KERNEL_ROOT_DIR)/include/arch/amd64
KERNEL_CC_INVOKE := $(KERNEL_CC) $(KERNEL_CFLAGS) $(KERNEL_DEFINES) $(KERNEL_INCLUDE)
KERNEL_CPP_SOURCES := %.cpp $(EFI_SHIM_SRC)/%.cpp
KERNEL_C_SOURCES := %.c
KERNEL_ASM_SOURCES := arch/$(ARCH)/%.S

all: bin/kernel.elf

rebuild: clean all

obj:
	@mkdir -p $@ $@/lib $@/lib/c $@/lib/cpp $@/init

bin:
	@mkdir -p $@

obj/%.o: $(KERNEL_ASM_SOURCES) | obj
	$(KERNEL_AS) $(KERNEL_ASFLAGS) -o $@ $<

obj/%.o: $(KERNEL_C_SOURCES) | obj
	$(KERNEL_CC_INVOKE) $< -o $@

obj/%.o: $(KERNEL_CPP_SOURCES) | obj
	$(KERNEL_CC_INVOKE) $< -o $@

bin/kernel.elf: $(KERNEL_OBJS) | bin
	$(KERNEL_LD) -nostdlib -z max-page-size=0x1000 -z noexecstack \
		-T link.ld $^ -o $@
#obj/**/*.o

clean:
	$(RM) -r obj
	$(RM) -r bin

-include $(wildcard obj/*.d)