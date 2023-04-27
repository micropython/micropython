ifneq ($(MKENV_INCLUDED),1)
# We assume that mkenv is in the same directory as this file.
THIS_MAKEFILE = $(lastword $(MAKEFILE_LIST))
include $(dir $(THIS_MAKEFILE))mkenv.mk
endif

HELP_BUILD_ERROR ?= "See \033[1;31mhttps://github.com/micropython/micropython/wiki/Build-Troubleshooting\033[0m"
HELP_MPY_LIB_SUBMODULE ?= "\033[1;31mError: micropython-lib submodule is not initialized.\033[0m Run 'make submodules'"

# Extra deps that need to happen before object compilation.
OBJ_EXTRA_ORDER_DEPS =

# Generate header files.
OBJ_EXTRA_ORDER_DEPS += $(HEADER_BUILD)/moduledefs.h $(HEADER_BUILD)/root_pointers.h

ifeq ($(MICROPY_ROM_TEXT_COMPRESSION),1)
# If compression is enabled, trigger the build of compressed.data.h...
OBJ_EXTRA_ORDER_DEPS += $(HEADER_BUILD)/compressed.data.h
# ...and enable the MP_COMPRESSED_ROM_TEXT macro (used by MP_ERROR_TEXT).
CFLAGS += -DMICROPY_ROM_TEXT_COMPRESSION=1
endif

# QSTR generation uses the same CFLAGS, with these modifications.
QSTR_GEN_FLAGS = -DNO_QSTR
# Note: := to force evalulation immediately.
QSTR_GEN_CFLAGS := $(CFLAGS)
QSTR_GEN_CFLAGS += $(QSTR_GEN_FLAGS)
QSTR_GEN_CXXFLAGS := $(CXXFLAGS)
QSTR_GEN_CXXFLAGS += $(QSTR_GEN_FLAGS)

# This file expects that OBJ contains a list of all of the object files.
# The directory portion of each object file is used to locate the source
# and should not contain any ..'s but rather be relative to the top of the
# tree.
#
# So for example, py/map.c would have an object file name py/map.o
# The object files will go into the build directory and maintain the same
# directory structure as the source tree. So the final dependency will look
# like this:
#
# build/py/map.o: py/map.c
#
# We set vpath to point to the top of the tree so that the source files
# can be located. By following this scheme, it allows a single build rule
# to be used to compile all .c files.

vpath %.S . $(TOP) $(USER_C_MODULES)
$(BUILD)/%.o: %.S
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

vpath %.s . $(TOP) $(USER_C_MODULES)
$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(Q)$(AS) -o $@ $<

define compile_c
$(ECHO) "CC $<"
$(Q)$(CC) $(CFLAGS) -c -MD -o $@ $< || (echo -e $(HELP_BUILD_ERROR); false)
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

define compile_cxx
$(ECHO) "CXX $<"
$(Q)$(CXX) $(CXXFLAGS) -c -MD -o $@ $< || (echo -e $(HELP_BUILD_ERROR); false)
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

vpath %.c . $(TOP) $(USER_C_MODULES)
$(BUILD)/%.o: %.c
	$(call compile_c)

vpath %.cpp . $(TOP) $(USER_C_MODULES)
$(BUILD)/%.o: %.cpp
	$(call compile_cxx)

$(BUILD)/%.pp: %.c
	$(ECHO) "PreProcess $<"
	$(Q)$(CPP) $(CFLAGS) -Wp,-C,-dD,-dI -o $@ $<

# The following rule uses | to create an order only prerequisite. Order only
# prerequisites only get built if they don't exist. They don't cause timestamp
# checking to be performed.
#
# We don't know which source files actually need the generated.h (since
# it is #included from str.h). The compiler generated dependencies will cause
# the right .o's to get recompiled if the generated.h file changes. Adding
# an order-only dependency to all of the .o's will cause the generated .h
# to get built before we try to compile any of them.
$(OBJ): | $(HEADER_BUILD)/qstrdefs.generated.h $(HEADER_BUILD)/mpversion.h $(OBJ_EXTRA_ORDER_DEPS)

# The logic for qstr regeneration (applied by makeqstrdefs.py) is:
# - if anything in QSTR_GLOBAL_DEPENDENCIES is newer, then process all source files ($^)
# - else, if list of newer prerequisites ($?) is not empty, then process just these ($?)
# - else, process all source files ($^) [this covers "make -B" which can set $? to empty]
# See more information about this process in docs/develop/qstr.rst.
$(HEADER_BUILD)/qstr.i.last: $(SRC_QSTR) $(QSTR_GLOBAL_DEPENDENCIES) | $(QSTR_GLOBAL_REQUIREMENTS)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py pp $(CPP) output $(HEADER_BUILD)/qstr.i.last cflags $(QSTR_GEN_CFLAGS) cxxflags $(QSTR_GEN_CXXFLAGS) sources $^ dependencies $(QSTR_GLOBAL_DEPENDENCIES) changed_sources $?

$(HEADER_BUILD)/qstr.split: $(HEADER_BUILD)/qstr.i.last
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py split qstr $< $(HEADER_BUILD)/qstr _
	$(Q)$(TOUCH) $@

$(QSTR_DEFS_COLLECTED): $(HEADER_BUILD)/qstr.split
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py cat qstr _ $(HEADER_BUILD)/qstr $@

# Module definitions via MP_REGISTER_MODULE.
$(HEADER_BUILD)/moduledefs.split: $(HEADER_BUILD)/qstr.i.last
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py split module $< $(HEADER_BUILD)/module _
	$(Q)$(TOUCH) $@

$(HEADER_BUILD)/moduledefs.collected: $(HEADER_BUILD)/moduledefs.split
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py cat module _ $(HEADER_BUILD)/module $@

# Module definitions via MP_REGISTER_ROOT_POINTER.
$(HEADER_BUILD)/root_pointers.split: $(HEADER_BUILD)/qstr.i.last
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py split root_pointer $< $(HEADER_BUILD)/root_pointer _
	$(Q)$(TOUCH) $@

$(HEADER_BUILD)/root_pointers.collected: $(HEADER_BUILD)/root_pointers.split
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py cat root_pointer _ $(HEADER_BUILD)/root_pointer $@

# Compressed error strings.
$(HEADER_BUILD)/compressed.split: $(HEADER_BUILD)/qstr.i.last
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py split compress $< $(HEADER_BUILD)/compress _
	$(Q)$(TOUCH) $@

$(HEADER_BUILD)/compressed.collected: $(HEADER_BUILD)/compressed.split
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py cat compress _ $(HEADER_BUILD)/compress $@

# $(sort $(var)) removes duplicates
#
# The net effect of this, is it causes the objects to depend on the
# object directories (but only for existence), and the object directories
# will be created if they don't exist.
OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	$(MKDIR) -p $@

$(HEADER_BUILD):
	$(MKDIR) -p $@

ifneq ($(MICROPY_MPYCROSS_DEPENDENCY),)
# to automatically build mpy-cross, if needed
$(MICROPY_MPYCROSS_DEPENDENCY):
	$(MAKE) -C $(abspath $(dir $@)..)
endif

ifneq ($(FROZEN_DIR),)
$(error Support for FROZEN_DIR was removed. Please use manifest.py instead, see https://docs.micropython.org/en/latest/reference/manifest.html)
endif

ifneq ($(FROZEN_MPY_DIR),)
$(error Support for FROZEN_MPY_DIR was removed. Please use manifest.py instead, see https://docs.micropython.org/en/latest/reference/manifest.html)
endif

ifneq ($(FROZEN_MANIFEST),)
# If we're using the default submodule path for micropython-lib, then make
# sure it's included in "make submodules".
ifeq ($(MPY_LIB_DIR),$(MPY_LIB_SUBMODULE_DIR))
GIT_SUBMODULES += lib/micropython-lib
endif

# to build frozen_content.c from a manifest
$(BUILD)/frozen_content.c: FORCE $(BUILD)/genhdr/qstrdefs.generated.h $(BUILD)/genhdr/root_pointers.h | $(MICROPY_MPYCROSS_DEPENDENCY)
	$(Q)test -e "$(MPY_LIB_DIR)/README.md" || (echo -e $(HELP_MPY_LIB_SUBMODULE); false)
	$(Q)$(MAKE_MANIFEST) -o $@ -v "MPY_DIR=$(TOP)" -v "MPY_LIB_DIR=$(MPY_LIB_DIR)" -v "PORT_DIR=$(shell pwd)" -v "BOARD_DIR=$(BOARD_DIR)" -b "$(BUILD)" $(if $(MPY_CROSS_FLAGS),-f"$(MPY_CROSS_FLAGS)",) --mpy-tool-flags="$(MPY_TOOL_FLAGS)" $(FROZEN_MANIFEST)
endif

ifneq ($(PROG),)
# Build a standalone executable (unix does this)

# The executable should have an .exe extension for builds targeting 'pure'
# Windows, i.e. msvc or mingw builds, but not when using msys or cygwin's gcc.
COMPILER_TARGET := $(shell $(CC) -dumpmachine)
ifneq (,$(findstring mingw,$(COMPILER_TARGET)))
PROG := $(PROG).exe
endif

all: $(BUILD)/$(PROG)

$(BUILD)/$(PROG): $(OBJ)
	$(ECHO) "LINK $@"
# Do not pass COPT here - it's *C* compiler optimizations. For example,
# we may want to compile using Thumb, but link with non-Thumb libc.
	$(Q)$(CC) -o $@ $^ $(LIB) $(LDFLAGS)
ifndef DEBUG
	$(Q)$(STRIP) $(STRIPFLAGS_EXTRA) $@
endif
	$(Q)$(SIZE) $$(find $(BUILD) -path "$(BUILD)/build/frozen*.o") $@

clean: clean-prog
clean-prog:
	$(RM) -f $(BUILD)/$(PROG)
	$(RM) -f $(BUILD)/$(PROG).map

.PHONY: clean-prog
endif

submodules:
	$(ECHO) "Updating submodules: $(GIT_SUBMODULES)"
ifneq ($(GIT_SUBMODULES),)
	$(Q)git submodule sync $(addprefix $(TOP)/,$(GIT_SUBMODULES))
	$(Q)git submodule update --init $(addprefix $(TOP)/,$(GIT_SUBMODULES))
endif
.PHONY: submodules

LIBMICROPYTHON = libmicropython.a

# We can execute extra commands after library creation using
# LIBMICROPYTHON_EXTRA_CMD. This may be needed e.g. to integrate
# with 3rd-party projects which don't have proper dependency
# tracking. Then LIBMICROPYTHON_EXTRA_CMD can e.g. touch some
# other file to cause needed effect, e.g. relinking with new lib.
lib $(BUILD)/$(LIBMICROPYTHON): $(OBJ)
	$(Q)$(AR) rcs $(BUILD)/$(LIBMICROPYTHON) $^
	$(LIBMICROPYTHON_EXTRA_CMD)

clean:
	$(RM) -rf $(BUILD) $(CLEAN_EXTRA)
.PHONY: clean

print-cfg:
	$(ECHO) "PY_SRC = $(PY_SRC)"
	$(ECHO) "BUILD  = $(BUILD)"
	$(ECHO) "OBJ    = $(OBJ)"
.PHONY: print-cfg

print-def:
	@$(ECHO) "The following defines are built into the $(CC) compiler"
	$(TOUCH) __empty__.c
	@$(CC) -E -Wp,-dM __empty__.c
	@$(RM) -f __empty__.c

-include $(OBJ:.o=.P)
