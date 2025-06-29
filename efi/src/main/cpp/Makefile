.PHONY: all clean rebuild

ARCH := amd64
EFI_TARGET := x86_64-unknown-linux-gnu
EFI_CC := clang
EFI_AS := x86_64-linux-gnu-gcc
EFI_PREFIX ?= /usr/local
EFI_BIN_FNAME := BOOTX64.EFI
EFI_OBJS := $(EFI_PREFIX)/lib/crt0-efi-x86_64.o obj/main.o obj/virtual.o obj/asm.o
EFI_DEFINES := -DQUIET
EFI_CFLAGS := -target $(EFI_TARGET) -ffreestanding -fno-stack-protector -mno-red-zone -fshort-wchar -nostdlib -fvisibility=hidden -c -MMD -MP
EFI_ASFLAGS := -ffreestanding -m64 -c -MMD -MP
INCLUDES := -I../../../../include
EFI_INCLUDES := -I../../../include -I$(EFI_PREFIX)/include/efi -I$(EFI_PREFIX)/include/efi/x86_64
EFI_INCLUDE := $(INCLUDES) $(EFI_INCLUDES)
EFI_CC_INVOKE := $(EFI_CC) $(EFI_CFLAGS) $(EFI_DEFINES) $(EFI_INCLUDE)

all: bin/$(EFI_BIN_FNAME)

rebuild: clean all

obj:
	@mkdir -p $@

bin:
	@mkdir -p $@

# obj/miniz.o: third_party/miniz.c | obj
# 	clang -target $(EFI_CFLAGS) \
# 		-Ithird_party/include -DMINIZ_NO_ZLIB_APIS -DMINIZ_NO_ARCHIVE_APIS -DMINIZ_NO_STDIO -DMINIZ_NO_MALLOC \
# 		$< -o $@

obj/%.o: arch/$(ARCH)/%.S | obj
	$(EFI_AS) $(EFI_ASFLAGS) -o $@ $<

obj/%.o: %.cpp | obj
	$(EFI_CC_INVOKE) $< -o $@

obj/main.so: $(EFI_OBJS) | obj
	x86_64-linux-gnu-ld \
		-nostdlib -znocombreloc -shared -Bsymbolic \
		-T $(EFI_PREFIX)/lib/elf_x86_64_efi.lds \
		-L$(EFI_PREFIX)/lib \
		$^ -lefi -lgnuefi -o $@

bin/$(EFI_BIN_FNAME): obj/main.so | bin
	x86_64-linux-gnu-objcopy -j .text -j .sdata -j .data \
		-j .dynamic -j .rodata \
		-j .rel* -j .rela* \
		--target=efi-app-x86_64 \
		obj/main.so bin/$(EFI_BIN_FNAME)

clean:
	$(RM) -r obj
	$(RM) -r bin

-include $(wildcard obj/*.d)