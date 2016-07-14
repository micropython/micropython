#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

#
# See readme.txt for the make API
#

# Copy this pseudo variable so we can path expand it
MLIST := $(MAKEFILE_LIST)

# Win32 Nasty - must convert all paths into a format make can handle
ifeq ($(basename $(OPT_OS)),win32)
  PATHEXPAND := ARCH XCC XCXX XAS XLD XOC XOD XSZ XAR PROJECT BUILDDIR SRC DEFS LIBS INCPATH LIBPATH MLIST $(PATHLIST)

  # First convert \'s to /'s
  $(foreach var,$(PATHEXPAND),$(eval $(var):=$$(subst \,/,$($(var)))))

  # For cygwin gmake - need to convert all absolute paths (mingw gmake doesn't need this)
  ifneq ($(findstring cygdrive,$(PATH)),)
    DRIVELETTERS := a b c d e f g h i j k l m n o p q r s t u v w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
    $(foreach drv,$(DRIVELETTERS),$(foreach var,$(PATHEXPAND),$(eval $(var):=$$(patsubst $(drv):%,/cygdrive/$(drv)%,$($(var))))))
  endif
endif

# Where are we
CURRENTDIR := $(dir $(abspath $(lastword $(MLIST))))

# Handle cpu specific options
ifneq ($(OPT_CPU),)
  include $(CURRENTDIR)cpu_$(OPT_CPU).mk
endif

# Path resolution - Functions to convert a source path to a object path and visa-versa
src_obj_fn := $$(1)
obj_src_fn := $$(1)
$(foreach var,$(PATHLIST),$(eval obj_src_fn := $$$$(patsubst $(var)/%,$$$$($(var))/%,$$(obj_src_fn))))
$(foreach var,$(PATHLIST),$(eval src_obj_fn := $$$$(patsubst $$$$($(var))/%,$(var)/%,$$(src_obj_fn))))
src_obj_fn := $$(subst :,_drv_drv_,$$(subst ../,_dot_dot/,$(src_obj_fn)))
obj_src_fn := $$(subst _drv_drv_,:,$$(subst _dot_dot/,../,$(obj_src_fn)))
$(eval src_obj=$(src_obj_fn))
$(eval obj_src=$(obj_src_fn))

# Add ARCH to each of the compiler programs
ifeq ($(XCC),)
  XCC = $(ARCH)gcc
endif
ifeq ($(XCXX),)
  XCXX = $(ARCH)g++
endif
ifeq ($(XAS),)
  XAS = $(ARCH)gcc -x assembler-with-cpp
endif
ifeq ($(XLD),)
  XLD = $(ARCH)gcc
endif
ifeq ($(XOC),)
  XOC = $(ARCH)objcopy
endif
ifeq ($(XOD),)
  XOD = $(ARCH)objdump
endif
ifeq ($(XSZ),)
  XSZ = $(ARCH)size
endif
ifeq ($(XAR),)
  XAR = $(ARCH)ar
endif

# Default project name is the project directory name
ifeq ($(PROJECT),)
  ifneq ($(firstword $(abspath $(firstword $(MLIST)))),$(lastword $(abspath $(firstword $(MLIST)))))
    $(error Your directory contains spaces. Gmake barfs at that. Please define PROJECT)
  endif
  PROJECT := $(notdir $(patsubst %/,%,$(dir $(abspath $(firstword $(MLIST))))))
endif

# Output directories
ifeq ($(BUILDDIR),)
  ifeq ($(MAKECMDGOALS),Debug)
    BUILDDIR = bin/Debug
  endif
  ifeq ($(MAKECMDGOALS),Release)
    BUILDDIR = bin/Release
  endif
  ifeq ($(MAKECMDGOALS),cleanDebug)
    BUILDDIR = bin/Debug
  endif
  ifeq ($(MAKECMDGOALS),cleanRelease)
    BUILDDIR = bin/Release
  endif
  ifeq ($(BUILDDIR),)
    BUILDDIR = .build
  endif
endif
OBJDIR  = $(BUILDDIR)/obj
DEPDIR  = $(BUILDDIR)/dep

# Output files
MAPFILE = $(BUILDDIR)/$(PROJECT).map
LIBFILE = $(BUILDDIR)/lib$(PROJECT).a
FAKEFILE= fakefile.o
EXEFILE =
ifeq ($(basename $(OPT_OS)),win32)
  EXEFILE = $(BUILDDIR)/$(PROJECT).exe
  TARGETS = $(EXEFILE)
endif
ifeq ($(basename $(OPT_OS)),linux)
  EXEFILE = $(BUILDDIR)/$(PROJECT)
  TARGETS = $(EXEFILE)
endif
ifeq ($(basename $(OPT_OS)),osx)
  EXEFILE = $(BUILDDIR)/$(PROJECT)
  TARGETS = $(EXEFILE)
endif
ifeq ($(EXEFILE),)
  LDFLAGS += -nostartfiles
  EXEFILE = $(BUILDDIR)/$(PROJECT).elf
  TARGETS = $(EXEFILE) $(BUILDDIR)/$(PROJECT).hex $(BUILDDIR)/$(PROJECT).bin $(BUILDDIR)/$(PROJECT).dmp elfstats
endif

# Generate our object file lists
OBJS_THUMB	   += $(addprefix $(OBJDIR)/,$(call src_obj,$(addsuffix .o,$(basename $(SRC_THUMB)))))
OBJS_NOTHUMB   += $(addprefix $(OBJDIR)/,$(call src_obj,$(addsuffix .o,$(basename $(SRC_NOTHUMB)))))
ifeq ($(OPT_THUMB),yes)
  OBJS_THUMB   += $(OBJS) $(addprefix $(OBJDIR)/,$(call src_obj,$(addsuffix .o,$(basename $(SRC)))))
else
  OBJS_NOTHUMB += $(OBJS) $(addprefix $(OBJDIR)/,$(call src_obj,$(addsuffix .o,$(basename $(SRC)))))
endif
ifneq ($(OBJS_THUMB),)
  ifneq ($(OBJS_NOTHUMB),)
    # Mixed ARM and THUMB mode - enabled only if needed because it kills performance.
    SRCFLAGS += -mthumb-interwork
    LDFLAGS  += -mthumb-interwork
    DEFS     += THUMB_PRESENT
  else
    # Pure THUMB mode, THUMB C code cannot be called by ARM asm code directly.
    LDFLAGS  += -mthumb
    DEFS     += THUMB_PRESENT THUMB_NO_INTERWORKING
	FAKEFILE= fakethumbfile.o
  endif
endif

# Handle make API options that affect compiler arguments
ifneq ($(OPT_NONSTANDARD_FLAGS),yes)
  SRCFLAGS += -fomit-frame-pointer -Wall -Wextra -Wstrict-prototypes -fverbose-asm
endif
ifeq ($(OPT_LINK_OPTIMIZE),yes)
  SRCFLAGS += -ffunction-sections -fdata-sections -fno-common -flto
  LDFLAGS += -Wl,--gc-sections
endif
ifeq ($(OPT_GENERATE_MAP),yes)
  LDFLAGS += -Wl,-Map=$(MAPFILE),--cref
endif
ifeq ($(OPT_GENERATE_LISTINGS),yes)
  CFLAGS   += -Wa,-alms=$(@:.o=.lst)
  CXXFLAGS += -Wa,-alms=$(@:.o=.lst)
  ASFLAGS  += -Wa,-amhls=$(@:.o=.lst)
endif
ifneq ($(LDSCRIPT),)
  LDFLAGS  += -T$(LDSCRIPT)
endif

# Generate dependency information
SRCFLAGS += -MMD -MP -MF $(DEPDIR)/$(@F).d

# Combine all our compiler arguments
SRCFLAGS += -I. $(patsubst %,-I%,$(INCPATH)) $(patsubst %,-D%,$(patsubst -D%,%,$(DEFS)))
LDFLAGS  += $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(patsubst -l%,%,$(LIBS)))

################# Targets ######################

.PHONY: builddirs fakefile.o fakethumbfile.o elfstats all exe lib clean Debug Release cleanDebug cleanRelease

# Many IDE's use these targets instead.
Debug Release:				all
cleanDebug cleanRelease:	clean

# Make a program or a library?
ifeq ($(OPT_MAKE_LIB),yes)
  all: lib
else
  all: exe
endif

exe: builddirs $(FAKEFILE) $(TARGETS)
lib: builddirs $(FAKEFILE) $(LIBFILE)

builddirs:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(DEPDIR)

$(FAKEFILE):
ifneq ($(OPT_VERBOSE_COMPILE),yes)
  ifneq ($(filter %.cpp,$(SRC) $(SRC_NOTHUMB) $(SRC_THUMB)),)
	@echo .
	@echo C++ Compiler Options..
	@echo $(XCXX) -c $(CPPFLAGS) $(CXXFLAGS) $(SRCFLAGS) $(@:.o=.cpp) -o $(OBJDIR)/$@
  else
   ifneq ($(filter %.c++,$(SRC) $(SRC_NOTHUMB) $(SRC_THUMB)),)
	@echo .
	@echo C++ Compiler Options..
	@echo $(XCXX) -c $(CPPFLAGS) $(CXXFLAGS) $(SRCFLAGS) $(@:.o=.c++) -o $(OBJDIR)/$@
   endif
  endif
  ifneq ($(filter %.c,$(SRC) $(SRC_NOTHUMB) $(SRC_THUMB)),)
	@echo .
	@echo C Compiler Options....
	@echo $(XCC) -c $(CPPFLAGS) $(CFLAGS) $(SRCFLAGS) $(@:.o=.c) -o $(OBJDIR)/$@
  endif
  ifneq ($(filter %.s,$(SRC) $(SRC_NOTHUMB) $(SRC_THUMB)),)
	@echo .
	@echo Assembler Options.....
	@echo $(XCC) -c $(CPPFLAGS) $(CFLAGS) $(SRCFLAGS) $(@:.o=.s) -o $(OBJDIR)/$@
  endif
  ifneq ($(OPT_MAKE_LIB),yes)
	@echo .
	@echo Linker Options........
	@echo $(XLD) $(LDFLAGS) $(OBJDIR)/$@ -o $(EXEFILE)
  endif
	@echo .
endif

fakethumbfile.o $(OBJS_THUMB):	SRCFLAGS += -mthumb -DTHUMB

elfstats:		$(EXEFILE)
	@echo .
ifeq ($(USE_VERBOSE_COMPILE),yes)
	$(XSZ) $<
else
	@$(XSZ) $<
endif

# Implicit Rules

.SECONDEXPANSION:
$(OBJDIR)/%.o : $$(call obj_src,%.c)
	@mkdir -p $(dir $@)
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	@echo .
	$(XCC) -c $(CPPFLAGS) $(CFLAGS) $(SRCFLAGS) $< -o $@
else
	@echo Compiling $<
	@$(XCC) -c $(CPPFLAGS) $(CFLAGS) $(SRCFLAGS) $< -o $@
endif

$(OBJDIR)/%.o : $$(call obj_src,%.cpp)
	@mkdir -p $(dir $@)
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	@echo .
	$(XCXX) -c $(CPPFLAGS) $(CXXFLAGS) $(SRCFLAGS) $< -o $@
else
	@echo Compiling $<
	@$(XCXX) -c $(CPPFLAGS) $(CXXFLAGS) $(SRCFLAGS) $< -o $@
endif

$(OBJDIR)/%.o : $$(call obj_src,%.c++)
	@mkdir -p $(dir $@)
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	@echo .
	$(XCXX) -c $(CPPFLAGS) $(CXXFLAGS) $(SRCFLAGS) $< -o $@
else
	@echo Compiling $<
	@$(XCXX) -c $(CPPFLAGS) $(CXXFLAGS) $(SRCFLAGS) $< -o $@
endif

$(OBJDIR)/%.o : $$(call obj_src,%.s)
	@mkdir -p $(dir $@)
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	@echo .
	$(XAS) -c $(CPPFLAGS) $(ASFLAGS) $(SRCFLAGS) $< -o $@
else
	@echo Compiling $<
	@$(XAS) -c $(CPPFLAGS) $(ASFLAGS) $(SRCFLAGS) $< -o $@
endif

$(EXEFILE): $(OBJS_THUMB) $(OBJS_NOTHUMB) $(LDSCRIPT)
	@mkdir -p $(dir $@)
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	@echo .
	$(XLD) $(OBJS_THUMB) $(OBJS_NOTHUMB) $(LDFLAGS) -o $@
else
	@echo Linking $@
	@$(XLD) $(OBJS_THUMB) $(OBJS_NOTHUMB) $(LDFLAGS) -o $@
endif
ifeq ($(OPT_COPY_EXE),yes)
	@cp $@ .
endif

$(LIBFILE): $(OBJS_THUMB) $(OBJS_NOTHUMB)
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	@echo .
	$(XAR) -r $@ $^
else
	@echo Creating Library $@
	@$(XAR) -r $@ $^
endif
ifeq ($(OPT_COPY_EXE),yes)
	@cp $@ .
endif

%.hex: %.elf
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	$(XOC) -O ihex $< $@
else
	@echo Creating $@
	@$(XOC) -O ihex $< $@
endif
ifeq ($(OPT_COPY_EXE),yes)
	@cp $@ .
endif

%.bin: %.elf
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	$(XOC) -O binary $< $@
else
	@echo Creating $@
	@$(XOC) -O binary $< $@
endif
ifeq ($(OPT_COPY_EXE),yes)
	@cp $@ .
endif

%.dmp: %.elf
ifeq ($(OPT_VERBOSE_COMPILE),yes)
	$(XOD) -x --syms $< > $@
else
	@echo Creating $@
	@$(XOD) -x --syms $< > $@
endif
ifeq ($(OPT_COPY_EXE),yes)
	@cp $@ .
endif

# Goodness knows why we would want this.
gcov:
	-mkdir gcov
	$(COV) -u $(subst /,\,$(SRC_NOTHUMB) $(SRC_THUMB))
	-mv *.gcov ./gcov

# Include the dependency files, should be the last of the makefile except for clean
-include $(shell mkdir -p $(DEPDIR) 2>/dev/null) $(wildcard $(DEPDIR)/*)

# Clean
clean:
	-rm -fR $(BUILDDIR)

# *** EOF ***
