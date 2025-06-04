.PHONY: all run clean debug kernel efi image initrd rebuild

DISK_IMG_SIZE := 64M
DISK_IMAGE := disk.img
MEM_SIZE := 512M
INITRD_IMG_FNAME := initrd.img
QEMU := qemu-system-x86_64
OVMF := /usr/share/OVMF/OVMF_CODE_4M.fd
QEMU_FLAGS := -m $(MEM_SIZE) \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF) \
		-drive file=$(DISK_IMAGE),format=raw,if=virtio \
		-echr 0x14 \
		-vnc :1,password=off -nographic
EFI_BIN_FNAME := BOOTX64.EFI
EFI_SRC := boot/efi
EFI_BIN := $(EFI_SRC)/bin
KERNEL_SRC := kernel
KERNEL_BIN := $(KERNEL_SRC)/bin
KERNEL_OBJ := $(KERNEL_SRC)/obj
INITRD_SRC := $(KERNEL_OBJ)/initrd
INITRD_FILES := $(KERNEL_BIN)/kernel.elf

all: image

rebuild: clean all

image: $(DISK_IMAGE)

initrd: $(EFI_BIN)/$(INITRD_IMG_FNAME)

kernel: $(KERNEL_BIN)/kernel.elf

efi: $(EFI_BIN)/$(EFI_BIN_FNAME)

run: $(DISK_IMAGE)
# Use C-t to enter qemu monitor
	$(QEMU) $(QEMU_FLAGS)

debug: $(DISK_IMAGE)
# Use C-t to enter qemu monitor
	$(QEMU) $(QEMU_FLAGS) -s -S

$(DISK_IMAGE): $(EFI_BIN)/$(EFI_BIN_FNAME) $(EFI_BIN)/$(INITRD_IMG_FNAME)
	qemu-img create -f raw $(DISK_IMAGE) $(DISK_IMG_SIZE)
	mkfs.fat -F 32 $(DISK_IMAGE)
	mmd -i $(DISK_IMAGE) ::/EFI
	mmd -i $(DISK_IMAGE) ::/EFI/BOOT
	mcopy -i $(DISK_IMAGE) $(EFI_BIN)/$(EFI_BIN_FNAME) ::/EFI/BOOT/
	mcopy -i $(DISK_IMAGE) $(EFI_BIN)/$(INITRD_IMG_FNAME) ::/

$(EFI_BIN)/$(INITRD_IMG_FNAME): $(INITRD_FILES)
	mkdir -p $(INITRD_SRC)
	rsync -a --delete $^ $(INITRD_SRC)/
	cd $(INITRD_SRC) && find . | cpio -o --format=newc > ../$(INITRD_IMG_FNAME)
	mv $(KERNEL_OBJ)/$(INITRD_IMG_FNAME) $(EFI_BIN)/
#	gzip -f $(EFI_BIN)/$(INITRD_IMG_FNAME)

$(KERNEL_BIN)/kernel.elf:
	cd $(KERNEL_SRC) && make

$(EFI_BIN)/$(EFI_BIN_FNAME):
	cd $(EFI_SRC) && make

clean:
	cd $(KERNEL_SRC) && make clean
	cd $(EFI_SRC) && make clean
	$(RM) $(DISK_IMAGE)
	$(RM) $(INITRD_SRC)