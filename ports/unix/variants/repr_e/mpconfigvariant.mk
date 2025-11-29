# build interpreter with "bigints" implemented as "longlong"

# otherwise, small int is essentially 64-bit
MICROPY_FORCE_32BIT := 1

MICROPY_PY_FFI := 0

RUN_TESTS_MPY_CROSS_FLAGS = --mpy-cross-flags='-march=x86 -msmall-int-bits=30'
