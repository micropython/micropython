# build interpreter with "bigints" implemented as "longlong"

# This needs to be built for a 32-bits target, otherwise small ints will
# essentially be 64-bit wide.

MICROPY_PY_FFI := 0

MPY_TOOL_FLAGS = -mlongint-impl longlong
