
MKCWD = mkdir -p $(@D)
BUILD = build
SYSROOT = sysroot

WARNINGS = 							\
	-Wall							\
	-Wextra							\
	-Werror							\

INCLUDES =							\
	-Isrc/libc/addons/				\
	-Isrc/libc/chadlibc/include/	\
	-Isrc/

DEFINES =							\
	-DSTB_SPRINTF_NOFLOAT			\
	-DSTB_SPRINTF_IMPLEMENTATION

DISABLED =							\
	-Wno-sign-compare				\
	-fno-builtin					\
	-fno-stack-protector			\
	-nostdlib 						\

CFLAGS = 							\
	$(WARNINGS)						\
	$(INCLUDES)						\
	$(DEFINES)						\
	$(DISABLED)						\
	-std=gnu2x						\
	-Os								\
	-ggdb3							\


ASFLAGS = 							\
	-F dwarf						\
	-g

LDFLAGS = 							\
	-nostdlib						\
	-static							\

CONF = python meta/config.py

export MENUCONFIG_STYLE?=aquatic

.PHONY: defconfig
defconfig:
	$(CONF) --defconfig
	$(CONF) --genmake .config.mk

.PHONY: menuconfig
menuconfig:
	$(CONF) --menuconfig
	$(CONF) --genmake .config.mk

ifeq (, $(wildcard ./.config))
.PHONY: all
all:
	@echo "Please run 'make defconfig' or 'make menuconfig' first"
else
include .config.mk
include src/arch/.build.mk
include src/libc/.build.mk
include src/loader/.build.mk
include src/alloc/.build.mk
include src/kernel/.build.mk

.PHONY: all
all: $(KERNEL)

endif

.PHONY: clean

clean:
	rm -rf $(BUILD) $(KERNEL) $(SYSROOT)

sysroot_skel:
	mkdir -p $(SYSROOT)/boot $(SYSROOT)/bin $(SYSROOT)/EFI/BOOT
	cp $(KERNEL) $(SYSROOT)/boot

sysroot: $(KERNEL) sysroot_skel $(LOADER_FILE)

run: sysroot
	$(EMU) $(EMUFLAGS)

.SUFFIXES:
.DEFAULT_GOAL: all