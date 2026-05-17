# build interpreter with "bigints" implemented as "longlong"

# otherwise, small int is essentially 64-bit
MICROPY_FORCE_32BIT := 1

MICROPY_PY_FFI := 0

MPY_TOOL_FLAGS = -mlongint-impl longlong
