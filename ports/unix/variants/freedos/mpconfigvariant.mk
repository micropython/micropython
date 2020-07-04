CC = i586-pc-msdosdjgpp-gcc

STRIP = i586-pc-msdosdjgpp-strip

SIZE = i586-pc-msdosdjgpp-size

CFLAGS += \
	-DMICROPY_NLR_SETJMP \
	-Dtgamma=gamma \
	-DMICROPY_EMIT_X86=0 \
	-DMICROPY_NO_ALLOCA=1 \

PROG = micropython-freedos

MICROPY_PY_SOCKET = 0
MICROPY_PY_FFI = 0
MICROPY_PY_JNI = 0
MICROPY_PY_BTREE = 0
MICROPY_PY_THREAD = 0
MICROPY_PY_USSL = 0
