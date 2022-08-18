KERNEL = $(BUILD)/kernel.elf
KERNEL_BUILD = $(BUILD)/kernel

KERNEL_CFLAGS +=                                        \
        $(CFLAGS)                                       \
        -fno-zero-initialized-in-bss    				\
        -mcmodel=kernel                                 \
        -D__kernel__									\
		-D__SCHED_QUANTUM__=$(CONFIG_SCHED_QUANTUM)		\

KERNEL_LDFLAGS +=                                       \
        $(LDFLAGS)                                      \

KERNEL_SRC +=                                           \
        $(wildcard src/kernel/*.c)              		\
        $(LIBC_SRC)

KERNEL_OBJ := $(patsubst %, $(KERNEL_BUILD)/%.o, $(KERNEL_SRC))
DEPENDENCIES += $(KERNEL_OBJ:.o=.d)

$(KERNEL_BUILD)/%.c.o: %.c
		@$(MKCWD)
		$(CROSS_CC) -c -o $@ $< $(KERNEL_CFLAGS)

$(KERNEL_BUILD)/%.s.o: %.s
		@$(MKCWD)
		$(CROSS_AS) -o $@ $< $(ASFLAGS)

$(KERNEL): $(KERNEL_OBJ)
		@$(MKCWD)
		$(CROSS_LD) -o $@ $^ $(KERNEL_LDFLAGS)