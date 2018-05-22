# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.

ifdef LONGINT_IMPL_NONE
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=none
endif

ifdef LONGINT_IMPL_MPZ
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz
endif

ifdef LONGINT_IMPL_LONGLONG
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=longlong
endif

ifndef MPY_TOOL_LONGINT_IMPL
$(error one of LONGINT_IMPL_NONE, LONGINT_IMPL_MPZ, or LONGINT_IMPL_LONGLONG must be defined)
endif

INTERNAL_LIBM = 1
