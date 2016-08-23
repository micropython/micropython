# List of available make goals:
#
# all                     Default target, builds the project
# clean                   Clean up the project
# rebuild                 Rebuild the project
# debug_flash             Builds the project and debug in flash
# debug_sram              Builds the project and debug in sram
#
# doc                     Build the documentation
# cleandoc                Clean up the documentation
# rebuilddoc              Rebuild the documentation
#
# \file
#
# Copyright (c) 2011 - 2014 Atmel Corporation. All rights reserved.
#
# \asf_license_start
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

# Include the config.mk file from the current working path, e.g., where the
# user called make.
include config.mk

# Tool to use to generate documentation from the source code
DOCGEN          ?= doxygen

# Look for source files relative to the top-level source directory
VPATH           := $(PRJ_PATH)

# Output target file
project_type    := $(PROJECT_TYPE)

# Output target file
ifeq ($(project_type),flash)
target          := $(TARGET_FLASH)
linker_script   := $(PRJ_PATH)/$(LINKER_SCRIPT_FLASH)
debug_script    := $(PRJ_PATH)/$(DEBUG_SCRIPT_FLASH)
else
target          := $(TARGET_SRAM)
linker_script   := $(PRJ_PATH)/$(LINKER_SCRIPT_SRAM)
debug_script    := $(PRJ_PATH)/$(DEBUG_SCRIPT_SRAM)
endif

# Output project name (target name minus suffix)
project         := $(basename $(target))

# Output target file (typically ELF or static library)
ifeq ($(suffix $(target)),.a)
target_type     := lib
else
ifeq ($(suffix $(target)),.elf)
target_type     := elf
else
$(error "Target type $(target_type) is not supported")
endif
endif

# Allow override of operating system detection. The user can add OS=Linux or
# OS=Windows on the command line to explicit set the host OS.
#
# This allows to work around broken uname utility on certain systems.
ifdef OS
  ifeq ($(strip $(OS)), Linux)
    os_type     := Linux
  endif
  ifeq ($(strip $(OS)), Windows)
    os_type     := windows32_64
  endif
endif

os_type         ?= $(strip $(shell uname))

ifeq ($(os_type),windows32)
os              := Windows
else
ifeq ($(os_type),windows64)
os              := Windows
else
ifeq ($(os_type),windows32_64)
os              ?= Windows
else
ifeq ($(os_type),)
os              := Windows
else
# Default to Linux style operating system. Both Cygwin and mingw are fully
# compatible (for this Makefile) with Linux.
os              := Linux
endif
endif
endif
endif

# Output documentation directory and configuration file.
docdir          := ../doxygen/html
doccfg          := ../doxygen/doxyfile.doxygen

CROSS           ?= arm-none-eabi-
AR              := $(CROSS)ar
AS              := $(CROSS)as
CC              := $(CROSS)gcc
CPP             := $(CROSS)gcc -E
CXX             := $(CROSS)g++
LD              := $(CROSS)g++
NM              := $(CROSS)nm
OBJCOPY         := $(CROSS)objcopy
OBJDUMP         := $(CROSS)objdump
SIZE            := $(CROSS)size
GDB             := $(CROSS)gdb

RM              := rm
ifeq ($(os),Windows)
RMDIR           := rmdir /S /Q
else
RMDIR           := rmdir -p --ignore-fail-on-non-empty
endif

# On Windows, we need to override the shell to force the use of cmd.exe
ifeq ($(os),Windows)
SHELL           := cmd
endif

# Strings for beautifying output
MSG_CLEAN_FILES         = "RM      *.o *.d"
MSG_CLEAN_DIRS          = "RMDIR   $(strip $(clean-dirs))"
MSG_CLEAN_DOC           = "RMDIR   $(docdir)"
MSG_MKDIR               = "MKDIR   $(dir $@)"

MSG_INFO                = "INFO    "
MSG_PREBUILD            = "PREBUILD  $(PREBUILD_CMD)"
MSG_POSTBUILD           = "POSTBUILD $(POSTBUILD_CMD)"

MSG_ARCHIVING           = "AR      $@"
MSG_ASSEMBLING          = "AS      $@"
MSG_BINARY_IMAGE        = "OBJCOPY $@"
MSG_COMPILING           = "CC      $@"
MSG_COMPILING_CXX       = "CXX     $@"
MSG_EXTENDED_LISTING    = "OBJDUMP $@"
MSG_IHEX_IMAGE          = "OBJCOPY $@"
MSG_LINKING             = "LN      $@"
MSG_PREPROCESSING       = "CPP     $@"
MSG_SIZE                = "SIZE    $@"
MSG_SYMBOL_TABLE        = "NM      $@"

MSG_GENERATING_DOC      = "DOXYGEN $(docdir)"

# Don't use make's built-in rules and variables
MAKEFLAGS       += -rR

# Don't print 'Entering directory ...'
MAKEFLAGS       += --no-print-directory

# Function for reversing the order of a list
reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

# Hide command output by default, but allow the user to override this
# by adding V=1 on the command line.
#
# This is inspired by the Kbuild system used by the Linux kernel.
ifdef V
  ifeq ("$(origin V)", "command line")
    VERBOSE = $(V)
  endif
endif
ifndef VERBOSE
  VERBOSE = 0
endif

ifeq ($(VERBOSE), 1)
  Q =
else
  Q = @
endif

arflags-gnu-y           := $(ARFLAGS)
asflags-gnu-y           := $(ASFLAGS)
cflags-gnu-y            := $(CFLAGS)
cxxflags-gnu-y          := $(CXXFLAGS)
cppflags-gnu-y          := $(CPPFLAGS)
cpuflags-gnu-y          :=
dbgflags-gnu-y          := $(DBGFLAGS)
libflags-gnu-y          := $(foreach LIB,$(LIBS),-l$(LIB))
ldflags-gnu-y           := $(LDFLAGS)
flashflags-gnu-y        :=
clean-files             :=
clean-dirs              :=

clean-files             += $(wildcard $(target) $(project).map)
clean-files             += $(wildcard $(project).hex $(project).bin)
clean-files             += $(wildcard $(project).lss $(project).sym)
clean-files             += $(wildcard $(build))

# Use pipes instead of temporary files for communication between processes
cflags-gnu-y    += -pipe
asflags-gnu-y   += -pipe
ldflags-gnu-y   += -pipe

# Archiver flags.
arflags-gnu-y   += rcs

# Always enable warnings. And be very careful about implicit
# declarations.
cflags-gnu-y    += -Wall -Wstrict-prototypes -Wmissing-prototypes
cflags-gnu-y    += -Werror-implicit-function-declaration
cxxflags-gnu-y  += -Wall
# IAR doesn't allow arithmetic on void pointers, so warn about that.
cflags-gnu-y    += -Wpointer-arith
cxxflags-gnu-y  += -Wpointer-arith

# Preprocessor flags.
cppflags-gnu-y  += $(foreach INC,$(addprefix $(PRJ_PATH)/,$(INC_PATH)),-I$(INC))
asflags-gnu-y   += $(foreach INC,$(addprefix $(PRJ_PATH)/,$(INC_PATH)),'-Wa,-I$(INC)')

# CPU specific flags.
cpuflags-gnu-y  += -mcpu=$(ARCH) -mthumb -D=__$(PART)__

# Dependency file flags.
depflags        = -MD -MP -MQ $@

# Debug specific flags.
ifdef BUILD_DEBUG_LEVEL
dbgflags-gnu-y  += -g$(BUILD_DEBUG_LEVEL)
else
dbgflags-gnu-y  += -g3
endif

# Optimization specific flags.
ifdef BUILD_OPTIMIZATION
optflags-gnu-y  = -O$(BUILD_OPTIMIZATION)
else
optflags-gnu-y  = $(OPTIMIZATION)
endif

# Always preprocess assembler files.
asflags-gnu-y   += -x assembler-with-cpp
# Compile C files using the GNU99 standard.
cflags-gnu-y    += -std=gnu99
# Compile C++ files using the GNU++98 standard.
cxxflags-gnu-y  += -std=gnu++98

# Don't use strict aliasing (very common in embedded applications).
cflags-gnu-y    += -fno-strict-aliasing
cxxflags-gnu-y  += -fno-strict-aliasing

# Separate each function and data into its own separate section to allow
# garbage collection of unused sections.
cflags-gnu-y    += -ffunction-sections -fdata-sections
cxxflags-gnu-y  += -ffunction-sections -fdata-sections

# Various cflags.
cflags-gnu-y += -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int
cflags-gnu-y += -Wmain -Wparentheses
cflags-gnu-y += -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused
cflags-gnu-y += -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef
cflags-gnu-y += -Wshadow -Wbad-function-cast -Wwrite-strings
cflags-gnu-y += -Wsign-compare -Waggregate-return
cflags-gnu-y += -Wmissing-declarations
cflags-gnu-y += -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations
cflags-gnu-y += -Wpacked -Wredundant-decls -Wnested-externs -Wlong-long
cflags-gnu-y += -Wunreachable-code
cflags-gnu-y += -Wcast-align
cflags-gnu-y += --param max-inline-insns-single=500

# To reduce application size use only integer printf function.
cflags-gnu-y += -Dprintf=iprintf

# Use newlib-nano to reduce application size
ldflags-gnu-y   += --specs=nano.specs

# Garbage collect unreferred sections when linking.
ldflags-gnu-y   += -Wl,--gc-sections

# Use the linker script if provided by the project.
ifneq ($(strip $(linker_script)),)
ldflags-gnu-y   += -Wl,-T $(linker_script)
endif

# Output a link map file and a cross reference table
ldflags-gnu-y   += -Wl,-Map=$(project).map,--cref

# Add library search paths relative to the top level directory.
ldflags-gnu-y   += $(foreach _LIB_PATH,$(addprefix $(PRJ_PATH)/,$(LIB_PATH)),-L$(_LIB_PATH))

a_flags  = $(cpuflags-gnu-y) $(depflags) $(cppflags-gnu-y) $(asflags-gnu-y) -D__ASSEMBLY__
c_flags  = $(cpuflags-gnu-y) $(dbgflags-gnu-y) $(depflags) $(optflags-gnu-y) $(cppflags-gnu-y) $(cflags-gnu-y)
cxx_flags= $(cpuflags-gnu-y) $(dbgflags-gnu-y) $(depflags) $(optflags-gnu-y) $(cppflags-gnu-y) $(cxxflags-gnu-y)
l_flags  = -Wl,--entry=Reset_Handler -Wl,--cref $(cpuflags-gnu-y) $(optflags-gnu-y) $(ldflags-gnu-y)
ar_flags = $(arflags-gnu-y)

# Source files list and part informations must already be included before
# running this makefile

# If a custom build directory is specified, use it -- force trailing / in directory name.
ifdef BUILD_DIR
	build-dir       := $(dir $(BUILD_DIR))$(if $(notdir $(BUILD_DIR)),$(notdir $(BUILD_DIR))/)
else
	build-dir        =
endif

# Create object files list from source files list.
obj-y                   := $(addprefix $(build-dir), $(addsuffix .o,$(basename $(CSRCS) $(ASSRCS))))
# Create dependency files list from source files list.
dep-files               := $(wildcard $(foreach f,$(obj-y),$(basename $(f)).d))

clean-files             += $(wildcard $(obj-y))
clean-files             += $(dep-files)

clean-dirs              += $(call reverse,$(sort $(wildcard $(dir $(obj-y)))))

# Default target.
.PHONY: all
ifeq ($(project_type),all)
all:
	$(MAKE) all PROJECT_TYPE=flash
	$(MAKE) all PROJECT_TYPE=sram
else
ifeq ($(target_type),lib)
all: $(target) $(project).lss $(project).sym
else
ifeq ($(target_type),elf)
all: prebuild $(target) $(project).lss $(project).sym $(project).hex $(project).bin postbuild
endif
endif
endif

prebuild:
ifneq ($(strip $(PREBUILD_CMD)),)
	@echo $(MSG_PREBUILD)
	$(Q)$(PREBUILD_CMD)
endif

postbuild:
ifneq ($(strip $(POSTBUILD_CMD)),)
	@echo $(MSG_POSTBUILD)
	$(Q)$(POSTBUILD_CMD)
endif

# Clean up the project.
.PHONY: clean
clean:
	@$(if $(strip $(clean-files)),echo $(MSG_CLEAN_FILES))
	$(if $(strip $(clean-files)),$(Q)$(RM) $(clean-files),)
	@$(if $(strip $(clean-dirs)),echo $(MSG_CLEAN_DIRS))
# Remove created directories, and make sure we only remove existing
# directories, since recursive rmdir might help us a bit on the way.
ifeq ($(os),Windows)
	$(Q)$(if $(strip $(clean-dirs)),                        \
			$(RMDIR) $(strip $(subst /,\,$(clean-dirs))))
else
	$(Q)$(if $(strip $(clean-dirs)),                        \
		for directory in $(strip $(clean-dirs)); do     \
			if [ -d "$$directory" ]; then           \
				$(RMDIR) $$directory;           \
			fi                                      \
		done                                            \
	)
endif

# Rebuild the project.
.PHONY: rebuild
rebuild: clean all

# Debug the project in flash.
.PHONY: debug_flash
debug_flash: all
	$(GDB) -x "$(PRJ_PATH)/$(DEBUG_SCRIPT_FLASH)" -ex "reset" -readnow -se $(TARGET_FLASH)

# Debug the project in sram.
.PHONY: debug_sram
debug_sram: all
	$(GDB) -x "$(PRJ_PATH)/$(DEBUG_SCRIPT_SRAM)" -ex "reset" -readnow -se $(TARGET_SRAM)

.PHONY: objfiles
objfiles: $(obj-y)

# Create object files from C source files.
$(build-dir)%.o: %.c $(MAKEFILE_PATH) config.mk
	$(Q)test -d $(dir $@) || echo $(MSG_MKDIR)
ifeq ($(os),Windows)
	$(Q)test -d $(patsubst %/,%,$(dir $@)) || mkdir $(subst /,\,$(dir $@))
else
	$(Q)test -d $(dir $@) || mkdir -p $(dir $@)
endif
	@echo $(MSG_COMPILING)
	$(Q)$(CC) $(c_flags) -c $< -o $@

# Create object files from C++ source files.
$(build-dir)%.o: %.cpp $(MAKEFILE_PATH) config.mk
	$(Q)test -d $(dir $@) || echo $(MSG_MKDIR)
ifeq ($(os),Windows)
	$(Q)test -d $(patsubst %/,%,$(dir $@)) || mkdir $(subst /,\,$(dir $@))
else
	$(Q)test -d $(dir $@) || mkdir -p $(dir $@)
endif
	@echo $(MSG_COMPILING_CXX)
	$(Q)$(CXX) $(cxx_flags) -c $< -o $@

# Preprocess and assemble: create object files from assembler source files.
$(build-dir)%.o: %.S $(MAKEFILE_PATH) config.mk
	$(Q)test -d $(dir $@) || echo $(MSG_MKDIR)
ifeq ($(os),Windows)
	$(Q)test -d $(patsubst %/,%,$(dir $@)) || mkdir $(subst /,\,$(dir $@))
else
	$(Q)test -d $(dir $@) || mkdir -p $(dir $@)
endif
	@echo $(MSG_ASSEMBLING)
	$(Q)$(CC) $(a_flags) -c $< -o $@

# Include all dependency files to add depedency to all header files in use.
include $(dep-files)

ifeq ($(target_type),lib)
# Archive object files into an archive
$(target): $(MAKEFILE_PATH) config.mk $(obj-y)
	@echo $(MSG_ARCHIVING)
	$(Q)$(AR) $(ar_flags) $@ $(obj-y)
	@echo $(MSG_SIZE)
	$(Q)$(SIZE) -Bxt $@
else
ifeq ($(target_type),elf)
# Link the object files into an ELF file. Also make sure the target is rebuilt
# if the common Makefile.sam.in or project config.mk is changed.
$(target): $(linker_script) $(MAKEFILE_PATH) config.mk $(obj-y)
	@echo $(MSG_LINKING)
	$(Q)$(LD) $(l_flags) $(obj-y) $(libflags-gnu-y) -o $@
	@echo $(MSG_SIZE)
	$(Q)$(SIZE) -Ax $@
	$(Q)$(SIZE) -Bx $@
endif
endif

# Create extended function listing from target output file.
%.lss: $(target)
	@echo $(MSG_EXTENDED_LISTING)
	$(Q)$(OBJDUMP) -h -S $< > $@

# Create symbol table from target output file.
%.sym: $(target)
	@echo $(MSG_SYMBOL_TABLE)
	$(Q)$(NM) -n $< > $@

# Create Intel HEX image from ELF output file.
%.hex: $(target)
	@echo $(MSG_IHEX_IMAGE)
	$(Q)$(OBJCOPY) -O ihex $(flashflags-gnu-y)  $< $@

# Create binary image from ELF output file.
%.bin: $(target)
	@echo $(MSG_BINARY_IMAGE)
	$(Q)$(OBJCOPY) -O binary $< $@

# Provide information about the detected host operating system.
.SECONDARY: info-os
info-os:
	@echo $(MSG_INFO)$(os) build host detected

# Build Doxygen generated documentation.
.PHONY: doc
doc:
	@echo $(MSG_GENERATING_DOC)
	$(Q)cd $(dir $(doccfg)) && $(DOCGEN) $(notdir $(doccfg))

# Clean Doxygen generated documentation.
.PHONY: cleandoc
cleandoc:
	@$(if $(wildcard $(docdir)),echo $(MSG_CLEAN_DOC))
	$(Q)$(if $(wildcard $(docdir)),$(RM) --recursive $(docdir))

# Rebuild the Doxygen generated documentation.
.PHONY: rebuilddoc
rebuilddoc: cleandoc doc
