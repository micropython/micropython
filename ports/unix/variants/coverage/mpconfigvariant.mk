PROG ?= micropython-coverage

# Disable optimisations and enable assert() on coverage builds.
DEBUG ?= 1

CFLAGS += \
	-fprofile-arcs -ftest-coverage \
	-Wformat -Wmissing-declarations -Wmissing-prototypes \
	-Wold-style-definition -Wpointer-arith -Wshadow -Wuninitialized -Wunused-parameter \
	-DMICROPY_UNIX_COVERAGE

LDFLAGS += -fprofile-arcs -ftest-coverage

FROZEN_MANIFEST = manifest_coverage.py

MICROPY_ROM_TEXT_COMPRESSION = 1
MICROPY_VFS_FAT = 1
MICROPY_VFS_LFS1 = 1
MICROPY_VFS_LFS2 = 1
MICROPY_PY_BLUETOOTH = 1
