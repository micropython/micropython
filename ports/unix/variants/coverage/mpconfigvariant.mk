PROG ?= micropython-coverage

# Disable optimisations and enable assert() on coverage builds.
DEBUG ?= 1

CFLAGS += \
	-fprofile-arcs -ftest-coverage \
	-Wformat -Wmissing-declarations -Wmissing-prototypes \
	-Wold-style-definition -Wpointer-arith -Wshadow -Wuninitialized -Wunused-parameter \
	-DMICROPY_UNIX_COVERAGE

LDFLAGS += -fprofile-arcs -ftest-coverage

MICROPY_VFS_FAT = 1
MICROPY_VFS_LFS1 = 1
MICROPY_VFS_LFS2 = 1

FROZEN_DIR=variants/coverage/frzstr
FROZEN_MPY_DIR=variants/coverage/frzmpy
