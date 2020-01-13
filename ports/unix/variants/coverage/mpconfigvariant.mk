PROG ?= micropython-coverage

COPT = -O0

CFLAGS_EXTRA += \
	-fprofile-arcs -ftest-coverage \
	-Wdouble-promotion -Wformat -Wmissing-declarations -Wmissing-prototypes -Wsign-compare \
	-Wold-style-definition -Wpointer-arith -Wshadow -Wuninitialized -Wunused-parameter \
	-DMICROPY_UNIX_COVERAGE

LDFLAGS_EXTRA += -fprofile-arcs -ftest-coverage

FROZEN_MANIFEST = manifest_coverage.py

MICROPY_VFS_FAT = 1
MICROPY_VFS_LFS1 = 1
MICROPY_VFS_LFS2 = 1
