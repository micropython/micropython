# Enable/disable modules and 3rd-party libs to be included in interpreter

# Build 32-bit binaries on a 64-bit host
MICROPY_FORCE_32BIT = 0

# This variable can take the following values:
#  0 - no readline, just simple input
#  1 - use MicroPython version of readline
#  2 - use GNU readline (causes binary to be licensed under GPL)
MICROPY_USE_READLINE = 1

# Subset of CPython time module
MICROPY_PY_TIME = 1

# Subset of CPython termios module
MICROPY_PY_TERMIOS = 1

# Subset of CPython socket module
MICROPY_PY_SOCKET = 1

# ffi module requires libffi (libffi-dev Debian package)
MICROPY_PY_FFI = 1

# ussl module requires axtls
MICROPY_PY_USSL = 0

# jni module requires JVM/JNI
MICROPY_PY_JNI = 0
