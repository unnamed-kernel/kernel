ifeq ($(CONFIG_ALLOC_LIBALLOC), y)
KERNEL_SRC += $(wildcard src/alloc/liballoc/*.c)
else
$(error "No allocator selected")
endif