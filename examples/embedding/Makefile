MICROPY_TOP ?= ../..

# Get the absolute path to the directory (so that all paths passed to
# libmicropython build can be absolute).
ABS_PATH = $(realpath .)

LIBMICROPYTHON_PORT=$(MICROPY_TOP)/ports/libmicropython
LIBMICROPYTHON_BUILD=$(ABS_PATH)/build-libmicropython
LIBMICROPYTHON_A=$(LIBMICROPYTHON_BUILD)/libmicropython.a

CFLAGS = -std=c99

# For mpconfigport.h libmicropython headers.
INC += -I$(LIBMICROPYTHON_PORT)

# For generated headers from the libmicropython build.
INC += -I$(LIBMICROPYTHON_BUILD)

# For all MicroPython headers.
INC += -I$(MICROPY_TOP)

CFLAGS += $(INC)

hello-embed: hello-embed.o $(LIBMICROPYTHON_A)

hello-embed.o: $(LIBMICROPYTHON_A)

V ?= 0

# Overrides:
#  BUILD to set the directory for compile artifacts.
#  LIBMICROPYTHON to set the output path for libmicropython.a
#  MICROPY_QSTRDEFSEMBED_H to use our custom qstrdefsembed.h
#  MICROPY_MPCONFIGEMBED_H to use our custom mpconfigembed.h
#  V to inherit our verbosity setting
$(LIBMICROPYTHON_A): qstrdefsembed.h mpconfigembed.h
	@$(MAKE) \
	    -C $(LIBMICROPYTHON_PORT) \
	    BUILD=$(LIBMICROPYTHON_BUILD) \
	    LIBMICROPYTHON=$(LIBMICROPYTHON_A) \
	    MICROPY_QSTRDEFSEMBED_H=$(ABS_PATH)/qstrdefsembed.h \
	    MICROPY_MPCONFIGEMBED_H=$(ABS_PATH)/mpconfigembed.h \
	    V=$(V) \
	    lib

clean:
	rm -rf *.o hello-embed $(LIBMICROPYTHON_BUILD)
