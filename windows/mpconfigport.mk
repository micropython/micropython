# Enable/disable modules and 3rd-party libs to be included in interpreter

# Build 32-bit binaries on a 64-bit host
MICROPY_FORCE_32BIT = 0

# Linking with GNU readline causes binary to be licensed under GPL
MICROPY_USE_READLINE = 1

# ffi module requires libffi (libffi-dev Debian package)
MICROPY_PY_FFI = 0
