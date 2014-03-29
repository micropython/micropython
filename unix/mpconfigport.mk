# Enable/disable modules and 3rd-party libs to be included in interpreter

# Linking with GNU readline causes binary to be licensed under GPL
MICROPY_USE_READLINE = 1

# Subset of CPython time module
MICROPY_MOD_TIME = 1

# ffi module requires libffi (libffi-dev Debian package)
MICROPY_MOD_FFI = 0
