
ifeq ($(CONFIG_LOADER_EFI), y)
	LOADER_FILE = $(SYSROOT)/EFI/BOOT/BOOTX64.EFI
else 
	LOADER_FILE = kernel.iso
endif

KERNEL_LDFLAGS = -Tsrc/loader/limine/linker.ld
KERNEL_SRC += $(wildcard src/loader/limine/*.c)
VER = 3.10.5

$(SYSROOT)/boot/limine.cfg:
	cp ./src/loader/limine/limine.cfg $(SYSROOT)/boot/

$(SYSROOT)/EFI/BOOT/BOOTX64.EFI: $(SYSROOT)/boot/limine.cfg
	wget https://github.com/limine-bootloader/limine/raw/v$(VER)-binary/BOOTX64.EFI -O $(SYSROOT)/EFI/BOOT/BOOTX64.EFI

./limine-deploy.c:
	wget https://github.com/limine-bootloader/limine/raw/v$(VER)-binary/limine-deploy.c -O limine-deploy.c

./limine-hhd.h:
	wget https://github.com/limine-bootloader/limine/raw/v$(VER)-binary/limine-hdd.h -O limine-hdd.h

./limine-deploy: ./limine-deploy.c ./limine-hhd.h
	cc -g -o2 -pipe -Wall -Wextra -std=c99 -D__USE_MINGW_ANSI_STDIO limine-deploy.c -o ./limine-deploy
	rm limine-deploy.c limine-hdd.h

$(SYSROOT)/boot/limine-cd.bin:
	wget https://github.com/limine-bootloader/limine/raw/v$(VER)-binary/limine-cd.bin -O $(SYSROOT)/boot/limine-cd.bin

$(SYSROOT)/boot/limine-cd-efi.bin:
	wget https://github.com/limine-bootloader/limine/raw/v$(VER)-binary/limine-cd-efi.bin -O $(SYSROOT)/boot/limine-cd-efi.bin

$(SYSROOT)/boot/limine.sys:
	wget https://github.com/limine-bootloader/limine/raw/v$(VER)-binary/limine.sys -O $(SYSROOT)/boot/limine.sys

kernel.iso: $(SYSROOT)/boot/limine.cfg ./limine-deploy $(SYSROOT)/boot/limine.sys $(SYSROOT)/boot/limine-cd.bin $(SYSROOT)/boot/limine-cd-efi.bin
	xorriso -as mkisofs -b /boot/limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot /boot/limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
		$(SYSROOT) -o kernel.iso
	
	./limine-deploy kernel.iso
	rm limine-deploy
	

