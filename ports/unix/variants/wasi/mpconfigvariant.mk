# Build for WASI

FROZEN_MANIFEST ?= $(VARIANT_DIR)/manifest.py

# WASI doesn't have FFI
MICROPY_PY_FFI=0

# When threading is enabled, micropython GC uses signals, which is
# not available on WASI.
MICROPY_PY_THREAD=0

# Disable for now because network support is limited in WASI.
MICROPY_PY_SOCKET=0
MICROPY_PY_SSL=0

# ../../lib/berkeley-db-1.xx/PORT/include/db.h:40:10:
# fatal error: 'sys/cdefs.h' file not found
MICROPY_PY_BTREE=0

# WASI doesn't have termios
MICROPY_PY_TERMIOS=0
MICROPY_USE_READLINE=0

# The following things might just work as they are.
# Disabled for now because I'm not interested in them.
MICROPY_VFS_FAT=0
MICROPY_VFS_LFS1=0
MICROPY_VFS_LFS2=0
