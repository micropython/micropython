# this makefile is a bit complex.
# the build process is as follows:
# 1. on first invocation, we don't have the CFLAGS_FILE. the makefile is copied to a temporary
#    CFLAGS_BUILD directory.
# 2. kernel module build is invoked on CFLAGS_BUILD.
# 3. the makefile is re-read during the kernel module build, then it generates grabs the CFLAGS and saves
#    them to CFLAGS_FILE.
# 4. this makefile is recursively invoked, and this time since it has the CFLAGS_FILE it will run MPY object
#    files build as usual.
# 5. for linking, this makefile invokes again a kernel module build, this time with the Kbuild file copied to
#    the BUILD directory. the Kbuild files will provide the list of MPY objects files.

MAKEFILE = $(lastword $(MAKEFILE_LIST))

# this is defined in mkenv.mk, but we need it now without the rest of mkenv.mk
BUILD ?= build

CFLAGS_BUILD = $(BUILD)/cflags
CFLAGS_BASENAME = cflags.txt
CFLAGS_FILE = $(CFLAGS_BUILD)/$(CFLAGS_BASENAME)

KDIR ?= /lib/modules/$(shell uname -r)/build

ifneq ($(KERNELRELEASE),)

# trick taken from kernel-cflags-finder, https://github.com/fishinabarrel/linux-kernel-module-rust

obj-m += dummy.o
clean-files := dummy.c

our_addtree = $(if $(patsubst -I%,%,$(1)), \
$(if $(filter-out -I/% -I../%,$(1)),$(patsubst ./%,$(CURDIR)/%,$(patsubst -I%,-I$(CURDIR)/%,$(1))),$(1)),$(1))
our_flags = $(foreach o,$($(1)),$(call our_addtree,$(o)))

$(M)/dummy.c:
	@echo $(NOSTDINC_FLAGS) $(call our_flags,LINUXINCLUDE) $(or $(__c_flags),$(_c_flags)) $(modkern_cflags) > $(M)/$(CFLAGS_BASENAME)
	@touch $@

.PHONY: $(M)/dummy.c

else

ifneq ($(shell test -e $(CFLAGS_FILE) && echo -n yes),yes)

all:
	mkdir -p $(CFLAGS_BUILD)
	cp $(MAKEFILE) $(CFLAGS_BUILD)/Makefile
	$(MAKE) -C $(KDIR) M=$(abspath $(CFLAGS_BUILD))
	# re-invoke make on the current file, now that we have the cflags.
	$(MAKE)

# simple clean rule, at this point we don't have the one provided by mkrules.mk
# there's nothing to clean now really, but it's still nice to have a "clean" rule
# that's usable all the time.
clean:
	rm -rf $(BUILD)

else

include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

CFLAGS = $(INC) -Wall -Werror -O3

ifneq ($(STRUCT_LAYOUT),)
FROZEN_MANIFEST = manifest.py
CFLAGS += -DINCLUDE_STRUCT_LAYOUT
MPY_CROSS_FLAGS += -msmall-int-bits=32 -X heapsize=4m
MPY_TOOL_FLAGS += -mmpz-dig-size=32
endif

# include py core make definitions
include $(TOP)/py/py.mk

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

KDIR ?= /lib/modules/$(shell uname -r)/build

LD = gcc

KERNEL_CFLAGS = $(shell cat "$(CFLAGS_FILE)")
# the kernel compiles with gnu89, MicroPython does not. gnu99 works for both.
KERNEL_CFLAGS := $(subst -std=gnu89,-std=gnu99,$(KERNEL_CFLAGS))
# disable "undef" warnings enabled by the kernel.
# (some MICROPY_* options are not always defined and are implicitly evaluated to 0)
KERNEL_CFLAGS := $(subst -Wundef,,$(KERNEL_CFLAGS))
# disable "declaration after statements" warnings enabled by the kernel, MicroPython does that
# all the time :)
KERNEL_CFLAGS := $(subst -Wdeclaration-after-statement,,$(KERNEL_CFLAGS))
# fix path of structleak plugin
KERNEL_CFLAGS := $(subst ./scripts/gcc-plugins/structleak_plugin.so,$(KDIR)/scripts/gcc-plugins/structleak_plugin.so,$(KERNEL_CFLAGS))
# finally! add the kernel cflags
CFLAGS += $(KERNEL_CFLAGS)
CSUPEROPT = -O3

SRC_C = \
	main.c \
	lib/utils/stdout_helpers.c \
	lib/utils/interrupt_char.c \
	lib/mp-readline/readline.c \
	lib/utils/pyexec.c \
	extmod/machine_mem.c \
	extmod/moductypes.c \
	modkernel_ffi.c \
	modmachine.c

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

SRC_QSTR += $(SRC_C)

include $(TOP)/py/mkrules.mk

$(BUILD)/Kbuild: Kbuild
	cp -f $< $@

$(BUILD)/mpy.ko: $(OBJ) $(BUILD)/Kbuild
# let the kernel makefile do the actual linking
	$(Q)$(MAKE) -C$(KDIR) M=$(abspath $(BUILD)) modules
	$(Q)$(SIZE) $@

all: $(BUILD)/mpy.ko

endif

endif
