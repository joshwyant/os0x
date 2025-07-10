.PHONY: all clean rebuild

ARCH := amd64
PROJ_ROOT_DIR := ..
PACKAGES_SRC := $(PROJ_ROOT_DIR)/packages
EFI_SHIM_SRC := $(PACKAGES_SRC)/efi_shim/src/main/cpp
EFI_SHIM_SOURCES := $(EFI_SHIM_SRC)/%.cpp
INCLUDES := -I$(PROJ_ROOT_DIR)/include
EFI_PREFIX ?= /usr/local
EFI_INCLUDES := -I$(EFI_PREFIX)/include/efi -I$(EFI_PREFIX)/include/efi/x86_64
CORE_SRC := $(PROJ_ROOT_DIR)/core/src/main/cpp
CORE_SOURCES := $(CORE_SRC)/%.cpp
CORE_OBJS := obj/core/format.o \
	obj/core/logging.o \
	obj/core/status.o \
	obj/core/stdlib/ostream.o
KERNEL_SRC := src/main/cpp
KERNEL_TARGET := x86_64-unknown-elf
KERNEL_AS := x86_64-linux-gnu-gcc
KERNEL_LD := x86_64-linux-gnu-ld
KERNEL_CC := clang
KERNEL_OBJS := 	obj/start.o \
				obj/asm.o \
				obj/main.o \
				obj/paging.o \
				obj/init.o \
				obj/lib/c/runtime_support.o \
				obj/lib/cpp/runtime_support.o \
				obj/lib/cpp/new.o \
				obj/packages/efi_shim/uefi_shim.o
KERNEL_DEFINES := 
KERNEL_CFLAGS := -target $(KERNEL_TARGET) -ffreestanding -fno-exceptions -fno-rtti -nostdlib -fno-builtin -fno-pic -mcmodel=large -g -c -MMD -MP -Wreturn-type -Wall -Werror
KERNEL_ASFLAGS := -ffreestanding -m64 -c -MMD -MP
KERNEL_ROOT_DIR := .
KERNEL_INCLUDE := $(INCLUDES) $(EFI_INCLUDES) -I$(KERNEL_ROOT_DIR)/include -I$(KERNEL_ROOT_DIR)/include/arch/amd64
KERNEL_CC_INVOKE := $(KERNEL_CC) $(KERNEL_CFLAGS) $(KERNEL_DEFINES) $(KERNEL_INCLUDE)
KERNEL_CPP_SOURCES := $(KERNEL_SRC)/%.cpp 
KERNEL_C_SOURCES := $(KERNEL_SRC)/%.c
KERNEL_ASM_SOURCES := $(KERNEL_SRC)/arch/$(ARCH)/%.S

all: bin/kernel.elf

rebuild: clean all

obj:
	@mkdir -p $@ \
		$@/lib $@/lib/c $@/lib/cpp \
		$@/packages/efi_shim \
		$@/core $@/core/stdlib

bin:
	@mkdir -p $@

obj/%.o: $(KERNEL_ASM_SOURCES) | obj
	$(KERNEL_AS) $(KERNEL_ASFLAGS) -o $@ $<

obj/%.o: $(KERNEL_C_SOURCES) | obj
	$(KERNEL_CC_INVOKE) $< -o $@

obj/%.o: $(KERNEL_CPP_SOURCES) | obj
	$(KERNEL_CC_INVOKE) $< -o $@

obj/packages/efi_shim/%.o: $(EFI_SHIM_SOURCES) | obj
	$(KERNEL_CC_INVOKE) $< -o $@

obj/core/%.o: $(CORE_SOURCES) | obj
	$(KERNEL_CC_INVOKE) $< -o $@

bin/kernel.elf: $(KERNEL_OBJS) $(CORE_OBJS) | bin
	$(KERNEL_LD) -nostdlib -z max-page-size=0x1000 -z noexecstack \
		-T link.ld $^ -o $@
#obj/**/*.o

clean:
	$(RM) -r obj
	$(RM) -r bin

-include $(wildcard obj/*.d)