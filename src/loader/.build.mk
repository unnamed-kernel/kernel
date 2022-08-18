ifeq ($(CONFIG_LOADER_LIMINE), y)
include src/loader/limine/.build.mk
else
$(error "No loader selected")
endif