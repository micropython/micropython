All make script files in this directory apply the following rules and assumptions:

- The scripts are written using gmake syntax
- They assume access to the following unix utilities
	rm, cp, mv, mkdir, sh
- They use and implement the following make variables

Input Variables (all optional unless otherwise specified)
----------------------------

OPT_MAKE_LIB=no|yes				- Make a library instead of an program = default no
OPT_VERBOSE_COMPILE=no|yes		- Turn on full compile messages - default no
OPT_GENERATE_LISTINGS=no|yes	- Generate listing files - default no
OPT_GENERATE_MAP=no|yes			- Generate a map file - default no
OPT_COPY_EXE=no|yes				- Copy the final program to the local project directory - default no
OPT_NONSTANDARD_FLAGS=no		- Turn off adding the standard compiler language flags - default no
OPT_LINK_OPTIMIZE=no			- Remove unused code/data during link - default no
OPT_OS=win32|win32.raw32|win32.chibios|linux|osx|chibios|freertos|ecos|raw32|rawrtos	- Mandatory: The operating system
OPT_CPU=x86|x64|stm32m1|stm32m4|stm32m7|at91sam7|armv6|raspberrypi	- Add some cpu dependant flags

BUILDDIR						- Build Directory - default is ".build" or "bin/Debug" or "bin/Release" depending on the target
PROJECT							- Project Name - default is the name of the project directory
PATHLIST						- A list of variable names of "master" paths that contain source and other objects of interest - default is ""

ARCH							- Architecture - default is ""
XCC								- C compiler - default is "$(ARCH)gcc"
XCXX							- C++ compiler - default is "$(ARCH)g++"
XAS								- Assembler - default is "$(ARCH)gcc -x assembler-with-cpp"
XLD								- Linker - default is "$(ARCH)gcc"
XOC								- Object Copy - default is "$(ARCH)objcopy"
XOD								- Object Dump - default is "$(ARCH)objdump"
XSZ								- Report binary dump details - default is "$(ARCH)size"
XAR								- Library archiver - default is "$(ARCH)ar"

SRCFLAGS						- Compiler defines for c, c++ and assembler files - default is ""
CFLAGS							- C specific compiler defines - default is ""
CXXFLAGS						- C++ specific compiler flags - default is ""
CPPFLAGS						- C Preprocessor flags for c, c++ and assembler files - default is ""
ASFLAGS							- Assembler specific compiler flags - default is ""
LDFLAGS							- Linker flags - default is ""

The following variables are a list of space separated values. In some cases an optional prefix (if specified) will be stripped off
the variables for compatibility with old definitions.

INCPATH							- List of header include directories - default is ""
LIBPATH							- List of library include directories - default is ""
DEFS							- List of preprocessor defines (any -D prefix is ignored) - default is ""
LIBS							- List of libraries (any -l prefix is ignored) - default is ""
SRC								- List of c, c++ and assembler source files - default is ""
OBJS							- List of additional object files - default is ""
LDSCRIPT						- Custom loader script - default is ""

ARM Specific options
----------------------------
OPT_THUMB=no|yes				- Compile normal sources in thumb mode - default is no

SRC_THUMB						- List of source files that MUST be compiled in thumb mode - default is ""
SRC_NOTHUMB						- List of source files that MUST be compiled in non-thumb mode - default is ""
OBJS_THUMB						- List of object files that MUST be linked in thumb mode - default is ""
OBJS_NOTHUMB					- List of object files that MUST be linked in non-thumb mode - default is ""

Targets
----------------------------

all
lib
exe
clean
Debug
cleanDebug
Release
cleanRelease
