LIBC_SRC = $(filter-out src/libc/chadlibc/src/math/sqrt.c, $(wildcard src/libc/addons/*.c) $(wildcard src/libc/chadlibc/src/*.c) $(wildcard src/libc/chadlibc/src/*/*.c))