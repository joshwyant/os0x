.PHONY: all clean rebuild

EFI_TARGET := x86_64-unknown-linux-gnu
EFI_CC := clang
EFI_PREFIX ?= /usr/local
EFI_BIN_FNAME := BOOTX64.EFI
EFI_SRC := .
EFI_OBJ := $(EFI_SRC)/obj
EFI_BIN := $(EFI_SRC)/bin
EFI_OBJS := $(EFI_PREFIX)/lib/crt0-efi-x86_64.o $(EFI_OBJ)/main.o
EFI_DEFINES := -DQUIET
EFI_CFLAGS := -target $(EFI_TARGET) -ffreestanding -fno-stack-protector -mno-red-zone -fshort-wchar -nostdlib -fvisibility=hidden -c -MMD -MP
INCLUDES := -I../../include
EFI_INCLUDES := -I../include -I$(EFI_PREFIX)/include/efi -I$(EFI_PREFIX)/include/efi/x86_64
EFI_INCLUDE := $(INCLUDES) $(EFI_INCLUDES) -I$(EFI_SRC)/include -I../../third_party/include
EFI_CC_INVOKE := $(EFI_CC) $(EFI_CFLAGS) $(EFI_DEFINES) $(EFI_INCLUDE)

all: $(EFI_BIN)/$(EFI_BIN_FNAME)

rebuild: clean all

$(EFI_OBJ):
	@mkdir -p $@

$(EFI_BIN):
	@mkdir -p $@

# $(EFI_OBJ)/miniz.o: third_party/miniz.c | $(EFI_OBJ)
# 	clang -target $(EFI_CFLAGS) \
# 		-Ithird_party/include -DMINIZ_NO_ZLIB_APIS -DMINIZ_NO_ARCHIVE_APIS -DMINIZ_NO_STDIO -DMINIZ_NO_MALLOC \
# 		$< -o $@

$(EFI_OBJ)/%.o: $(EFI_SRC)/%.c | $(EFI_OBJ)
	$(EFI_CC_INVOKE) $< -o $@

$(EFI_OBJ)/main.so: $(EFI_OBJS) | $(EFI_OBJ)
	x86_64-linux-gnu-ld \
		-nostdlib -znocombreloc -shared -Bsymbolic \
		-T $(EFI_PREFIX)/lib/elf_x86_64_efi.lds \
		-L$(EFI_PREFIX)/lib \
		$^ -lefi -lgnuefi -o $@

$(EFI_BIN)/$(EFI_BIN_FNAME): $(EFI_OBJ)/main.so | $(EFI_BIN)
	x86_64-linux-gnu-objcopy -j .text -j .sdata -j .data \
		-j .dynamic -j .rodata \
		-j .rel* -j .rela* \
		--target=efi-app-x86_64 \
		$(EFI_OBJ)/main.so $(EFI_BIN)/$(EFI_BIN_FNAME)

clean:
	$(RM) -r $(EFI_OBJ)
	$(RM) -r $(EFI_BIN)

-include $(wildcard $(EFI_OBJ)/*.d)