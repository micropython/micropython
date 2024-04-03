# Enable/disable modules and 3rd-party libs to be included in interpreter

# Build 32-bit binaries on a 64-bit host
MICROPY_FORCE_32BIT = 0

# This variable can take the following values:
#  0 - no readline, just simple stdin input
#  1 - use MicroPython version of readline
MICROPY_USE_READLINE = 1

# CIRCUITPY-CHANGE: not present
# btree module using Berkeley DB 1.xx
MICROPY_PY_BTREE = 0

# _thread module using pthreads
MICROPY_PY_THREAD = 1

# Subset of CPython termios module
MICROPY_PY_TERMIOS = 1

# CIRCUITPY-CHANGE: not present
# Subset of CPython socket module
MICROPY_PY_SOCKET = 0

# ffi module requires libffi (libffi-dev Debian package)
MICROPY_PY_FFI = 1

# CIRCUITPY-CHANGE: not present
# ssl module requires one of the TLS libraries below
MICROPY_PY_SSL = 0
# axTLS has minimal size but implements only a subset of modern TLS
# functionality, so may have problems with some servers.
MICROPY_SSL_AXTLS = 0
# mbedTLS is more up to date and complete implementation, but also
# more bloated.
# CIRCUITPY-CHANGE: not present
MICROPY_SSL_MBEDTLS = 0

# jni module requires JVM/JNI
MICROPY_PY_JNI = 0

# Avoid using system libraries, use copies bundled with MicroPython
# as submodules (currently affects only libffi).
MICROPY_STANDALONE = 0

# CIRCUITPY-CHANGE: not used
MICROPY_ROM_TEXT_COMPRESSION = 0

MICROPY_VFS_FAT = 1
# CIRCUITPY-CHANGE: not used
MICROPY_VFS_LFS1 = 0
MICROPY_VFS_LFS2 = 0

# CIRCUITPY-CHANGE
CIRCUITPY_ULAB = 1
CIRCUITPY_MESSAGE_COMPRESSION_LEVEL = 1
MICROPY_EMIT_NATIVE = 0
CFLAGS += -DCIRCUITPY=1
