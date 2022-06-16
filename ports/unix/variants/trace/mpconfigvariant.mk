PROG ?= micropython-trace

# Disable optimisations and enable assert() on trace builds.
DEBUG ?= 1

CFLAGS += \
	-Wformat -Wmissing-declarations -Wmissing-prototypes \
	-Wold-style-definition -Wpointer-arith -Wshadow -Wuninitialized -Wunused-parameter \

MICROPY_ROM_TEXT_COMPRESSION = 1
MICROPY_VFS_FAT = 1
MICROPY_VFS_LFS1 = 1
MICROPY_VFS_LFS2 = 1
