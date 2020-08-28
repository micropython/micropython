# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright 2020 NXP
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(micropython)

set(micropython_zephyr_dir   ${CMAKE_CURRENT_SOURCE_DIR})
set(micropython_top_dir      ${micropython_zephyr_dir}/../..)
set(micropython_build_dir    ${CMAKE_CURRENT_BINARY_DIR}/micropython)
set(micropython_lib          ${micropython_build_dir}/libmicropython.a)
set(micropython_configport_h ${micropython_build_dir}/mpconfigport.h)

# Get all the Zephyr include directories and cflags, and append them with what
# we need to build MicroPython
zephyr_get_include_directories_for_lang_as_string(C includes)
zephyr_get_system_include_directories_for_lang_as_string(C system_includes)
zephyr_get_compile_definitions_for_lang_as_string(C definitions)
zephyr_get_compile_options_for_lang_as_string(C options)

set(micropython_cflags
"${system_includes} ${includes} ${definitions} ${options} \
-I${micropython_build_dir} -I${micropython_zephyr_dir} -I${micropython_top_dir} \
-std=gnu99 -fomit-frame-pointer -DNDEBUG"
  )

# If we're calling make recursively, use the same make version as the top-level.
if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  set(submake "$(MAKE)")
else()
  set(submake "make")
endif()

# Construct a list of Zephyr port sources, relative to the top-level
# MicroPython directory.
list(APPEND micropython_zephyr_src_list
  main.c help.c moduos.c modusocket.c modutime.c modzephyr.c modzsensor.c
  modmachine.c machine_i2c.c machine_pin.c uart_core.c zephyr_storage.c
  )
list(TRANSFORM micropython_zephyr_src_list PREPEND ports/zephyr/)

# Construct a list of MicroPython lib sources used by the Zephyr port, relative
# to the top-level MicroPython directory.
list(APPEND micropython_lib_src_list
  timeutils/timeutils.c utils/mpirq.c utils/stdout_helpers.c utils/printf.c
  utils/pyexec.c utils/interrupt_char.c mp-readline/readline.c
  )
list(TRANSFORM micropython_lib_src_list PREPEND lib/)

# Combine the lists of sources into one string and substitute it into a
# generated Makefile in the build directory. The MicroPython lib target will
# process them for QSTR generation and build them.
list(APPEND micropython_src_list ${micropython_zephyr_src_list} ${micropython_lib_src_list})
list(JOIN micropython_src_list " \\\n\t" micropython_src)
configure_file(${micropython_zephyr_dir}/Makefile.in ${micropython_build_dir}/Makefile @ONLY)

# Copy the MicroPython configuration file set in Kconfig (usually
# mpconfigport.h, but could also be mpconfigport_minimal.h or some other
# user-defined file) into the build directory. This allows building
# libmicropython.a outside of the Zephyr port source directory, which is useful
# when building for multiple boards or regressing with the Zephyr sanitycheck
# script.
configure_file(${CONFIG_MICROPY_CONFIGFILE} ${micropython_configport_h} @ONLY)

# Use the generated Makefile to build libmicropython.a
include(ExternalProject)
ExternalProject_Add(
  micropython_project
  PREFIX ${micropython_build_dir}
  SOURCE_DIR ${micropython_zephyr_dir}
  BINARY_DIR ${micropython_build_dir}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND
  ${submake}
  CC=${CMAKE_C_COMPILER}
  AR=${CMAKE_AR}
  BUILD=${micropython_build_dir}
  MP_CONFIGFILE=<${micropython_configport_h}>
  FROZEN_DIR=${CONFIG_MICROPY_FROZEN_DIR}
  MICROPY_HEAP_SIZE=${CONFIG_MICROPY_HEAP_SIZE}
  MICROPY_VFS_FAT=$<BOOL:${CONFIG_MICROPY_VFS_FAT}>
  MICROPY_VFS_LFS1=$<BOOL:${CONFIG_MICROPY_VFS_LFS1}>
  MICROPY_VFS_LFS2=$<BOOL:${CONFIG_MICROPY_VFS_LFS2}>
  Z_CFLAGS=${micropython_cflags}
  DEPENDS zephyr_generated_headers
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${micropython_lib}
  )

# Create a wrapper CMake library that our application can link with
add_library(micropython_lib STATIC IMPORTED GLOBAL)
add_dependencies(micropython_lib micropython_project)
set_target_properties(micropython_lib PROPERTIES IMPORTED_LOCATION             ${micropython_lib})
set_target_properties(micropython_lib PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${micropython_top_dir})
set_target_properties(micropython_lib PROPERTIES ADDITIONAL_CLEAN_FILES        ${micropython_lib})

# Build the final application
target_sources(app PRIVATE src/zephyr_start.c src/zephyr_getchar.c)
target_link_libraries(app PUBLIC micropython_lib)
