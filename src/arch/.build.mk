ifeq ($(CONFIG_ARCH_X86-64), y)
include src/arch/x86_64/.build.mk
else
$(error "No architecture selected")
endif