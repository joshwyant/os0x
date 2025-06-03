.PHONY: run clean debug

ARCH := amd64
DISK_IMG_SIZE := 64M
DISK_IMAGE := disk.img
EFI_BIN_FNAME := BOOTX64.EFI
EFI_SRC := boot/efi
EFI_OBJ := $(EFI_SRC)/obj
EFI_BIN := $(EFI_SRC)/bin
KERNEL_SRC := kernel
KERNEL_OBJ := $(KERNEL_SRC)/obj
KERNEL_BIN := $(KERNEL_SRC)/bin
INITRD_IMG_FNAME := initrd.img
DEFINES := "-DQUIET"

all: $(DISK_IMAGE) run

run: $(DISK_IMAGE)
# Use C-t to enter qemu monitor
	qemu-system-x86_64 \
		-m 512M \
		-drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
		-drive file=$(DISK_IMAGE),format=raw,if=virtio \
		-echr 0x14 \
		-vnc :1,password=off -nographic

debug: $(DISK_IMAGE)
# Use C-t to enter qemu monitor
	qemu-system-x86_64 \
		-m 512M \
		-drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
		-drive file=$(DISK_IMAGE),format=raw,if=virtio \
		-echr 0x14 \
		-s -S \
		-nographic

$(DISK_IMAGE): $(EFI_BIN)/$(EFI_BIN_FNAME) $(EFI_BIN)/$(INITRD_IMG_FNAME)
	qemu-img create -f raw $(DISK_IMAGE) $(DISK_IMG_SIZE)
	mkfs.fat -F 32 $(DISK_IMAGE)
	mmd -i $(DISK_IMAGE) ::/EFI
	mmd -i $(DISK_IMAGE) ::/EFI/BOOT
	mcopy -i $(DISK_IMAGE) $(EFI_BIN)/$(EFI_BIN_FNAME) ::/EFI/BOOT/
	mcopy -i $(DISK_IMAGE) $(EFI_BIN)/$(INITRD_IMG_FNAME) ::/

# $(EFI_OBJ)/miniz.o: third_party/miniz.c
# 	mkdir -p $(EFI_OBJ)
# 	clang -target x86_64-unknown-linux-gnu \
# 		-DMINIZ_NO_ZLIB_APIS -DMINIZ_NO_ARCHIVE_APIS -DMINIZ_NO_STDIO -DMINIZ_NO_MALLOC \
# 		-ffreestanding -fno-stack-protector -mno-red-zone -fshort-wchar -nostdlib -fvisibility=hidden \
# 		-Ithird_party/include \
# 		-c $< -o $@

$(EFI_OBJ)/main.o: $(EFI_SRC)/main.c
	mkdir -p $(EFI_OBJ)
	clang -target x86_64-unknown-linux-gnu \
		-ffreestanding -fno-stack-protector \
		-fshort-wchar -mno-red-zone -fvisibility=hidden \
		-I/usr/local/include/efi -I/usr/local/include/efi/x86_64 \
		-Iboot/include -Ithird_party/include -Iinclude \
		$(DEFINES) \
		-c $< -o $@

$(EFI_OBJ)/main.so: $(EFI_OBJ)/main.o #$(EFI_OBJ)/miniz.o
	mkdir -p $(EFI_BIN)
	x86_64-linux-gnu-ld -nostdlib \
		-znocombreloc \
		-T /usr/local/lib/elf_x86_64_efi.lds \
		-shared -Bsymbolic \
		-L/usr/local/lib \
		/usr/local/lib/crt0-efi-x86_64.o $< \
		-lefi -lgnuefi -o $@

$(EFI_BIN)/$(EFI_BIN_FNAME): $(EFI_OBJ)/main.so
	mkdir -p $(EFI_BIN)
	x86_64-linux-gnu-objcopy -j .text -j .sdata -j .data \
		-j .dynamic -j .rodata \
		-j .rel* -j .rela* \
		--target=efi-app-x86_64 \
		$(EFI_OBJ)/main.so $(EFI_BIN)/$(EFI_BIN_FNAME)

$(KERNEL_OBJ)/asm.o: kernel/arch/$(ARCH)/asm.S
	mkdir -p $(KERNEL_OBJ)
	x86_64-linux-gnu-gcc -c -m64 -ffreestanding -o $@ $<

$(KERNEL_OBJ)/main.o: kernel/main.c
	mkdir -p $(KERNEL_OBJ)
	clang -target x86_64-unknown-elf -ffreestanding -nostdlib -fno-pic -c \
		-Ikernel/include \
		$< -o $@

$(KERNEL_OBJ)/boot.o: kernel/boot.c
	mkdir -p $(KERNEL_OBJ)
	clang -target x86_64-unknown-elf -ffreestanding -nostdlib -fno-pic -c \
		-Iboot/include -Ikernel/include \
		$< -o $@

$(KERNEL_BIN)/kernel.elf: $(KERNEL_OBJ)/boot.o $(KERNEL_OBJ)/main.o $(KERNEL_OBJ)/asm.o
	mkdir -p $(KERNEL_BIN)
	x86_64-linux-gnu-ld -nostdlib -z max-page-size=0x1000 -z noexecstack \
		-T kernel/link.ld -o $@ $(KERNEL_OBJ)/*.o

$(EFI_BIN)/$(INITRD_IMG_FNAME): $(KERNEL_BIN)/kernel.elf
	mkdir -p $(KERNEL_OBJ)/initrd
	cp $(KERNEL_BIN)/kernel.elf $(KERNEL_OBJ)/initrd/
	cd $(KERNEL_OBJ)/initrd && find . | cpio -o --format=newc > ../$(INITRD_IMG_FNAME)
	mv $(KERNEL_OBJ)/$(INITRD_IMG_FNAME) $(EFI_BIN)/
#	gzip -f $(EFI_BIN)/$(INITRD_IMG_FNAME)

clean:
	$(RM) $(DISK_IMAGE)
	$(RM) -r $(EFI_OBJ)
	$(RM) -r $(EFI_BIN)
	$(RM) -r $(KERNEL_OBJ)
	$(RM) -r $(KERNEL_BIN)