.PHONY: run clean debug

EFI_BINARY := BOOTX64.EFI
EFI_SRC := boot/efi
EFI_OBJ := $(EFI_SRC)/obj
IMAGE := disk.img
IMG_SIZE := 64M

all: $(IMAGE) run

run: $(IMAGE)
# Use C-t to enter qemu monitor
	qemu-system-x86_64 \
		-m 512M \
		-drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
		-drive file=$(IMAGE),format=raw,if=virtio \
		-echr 0x14 \
		-nographic

debug: $(IMAGE)
# Use C-t to enter qemu monitor
	qemu-system-x86_64 \
		-m 512M \
		-drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
		-drive file=$(IMAGE),format=raw,if=virtio \
		-echr 0x14 \
		-s -S \
		-nographic

$(IMAGE): $(EFI_SRC)/$(EFI_BINARY)
	qemu-img create -f raw $(IMAGE) $(IMG_SIZE)
	mkfs.fat -F 32 $(IMAGE)
	mmd -i $(IMAGE) ::/EFI
	mmd -i $(IMAGE) ::/EFI/BOOT
	mcopy -i $(IMAGE) $(EFI_SRC)/$(EFI_BINARY) ::/EFI/BOOT/

.SECONDARY: $(EFI_OBJ)/main.o $(EFI_OBJ)/main.so

$(EFI_OBJ)/main.o: $(EFI_SRC)/main.c
	mkdir -p $(EFI_OBJ)
	clang -target x86_64-unknown-linux-gnu \
		-ffreestanding -fno-stack-protector \
		-fshort-wchar -mno-red-zone -fvisibility=hidden \
		-I/usr/local/include/efi -I/usr/local/include/efi/x86_64 \
		-c $(EFI_SRC)/main.c -o $(EFI_OBJ)/main.o

$(EFI_OBJ)/main.so: $(EFI_OBJ)/main.o
	x86_64-linux-gnu-ld -nostdlib \
		-znocombreloc \
		-T /usr/local/lib/elf_x86_64_efi.lds \
		-shared -Bsymbolic \
		-L/usr/local/lib \
		/usr/local/lib/crt0-efi-x86_64.o $(EFI_OBJ)/main.o \
		-lefi -lgnuefi -o $(EFI_OBJ)/main.so

$(EFI_SRC)/$(EFI_BINARY): $(EFI_OBJ)/main.so
	x86_64-linux-gnu-objcopy -j .text -j .sdata -j .data \
		-j .dynamic -j .rodata \
		-j .rel* -j .rela* \
		--target=efi-app-x86_64 \
		$(EFI_OBJ)/main.so $(EFI_SRC)/$(EFI_BINARY)

clean:
	$(RM) $(IMAGE)
	$(RM) $(EFI_SRC)/BOOTX64.EFI
	$(RM) -r $(EFI_OBJ)