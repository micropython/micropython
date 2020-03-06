# The following is a temporary hack to forefully undefine vars that might have
# be defined by a calling Makefile (from recursive make).
# TODO: Find a better way to be able to call this Makefile recursively.
ifneq ($(findstring undefine,$(.FEATURES)),)
override undefine COPT
override undefine CFLAGS_EXTRA
override undefine LDFLAGS_EXTRA
override undefine FROZEN_DIR
override undefine FROZEN_MPY_DIR
override undefine BUILD
override undefine PROG
endif

include mpy-cross.mk
