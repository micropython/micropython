/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jim Mussared & Thorsten von Eicken
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// We need the MICROPY_PORT_FUN_TABLE #define from mpconfigport.h but need to exclude pulling in the
// rest of the .h world because it spoils the phony "voidfun" type decl below. We #define some
// XXX_H defines to prevent recursive inclusion.
#define _STDINT_H
#define _NEWLIB_ALLOCA_H
#define _SYS_TYPES_H
#define _ROM_ETS_SYS_H_
#define INC_FREERTOS_H
typedef int size_t;
typedef int int32_t;
typedef unsigned int uint32_t;
#include "mpconfigport.h"

#if MICROPY_PORT_FUN_TABLE

#if !MICROPY_ESP_IDF_4
// A bunch of functions don't exist in ESP-IDF v3.3, so we need dummies
#define esp_flash_encryption_init_checks mp_port_fun_does_not_exist
#define esp_get_flash_encryption_mode mp_port_fun_does_not_exist
#define esp_unregister_shutdown_handler mp_port_fun_does_not_exist
#define nvs_entry_find mp_port_fun_does_not_exist
#define nvs_entry_info mp_port_fun_does_not_exist
#define nvs_entry_next mp_port_fun_does_not_exist
#define nvs_release_iterator mp_port_fun_does_not_exist
#define sdmmc_io_get_cis_data mp_port_fun_does_not_exist
#define sdmmc_io_print_cis_info mp_port_fun_does_not_exist
#define sdspi_host_io_int_enable mp_port_fun_does_not_exist
#define sdspi_host_io_int_wait mp_port_fun_does_not_exist
#endif

#include "portnativeglue.h"

// mp_port_fun_table below is a table of C functions to which a dynamically loaded native module can
// link. Typically it is populated with ESP-IDF functions, but it can really contain any 'extern' C
// function.
// The bulk of the table comes from port_fun.inc, which is a file of comma-separated function
// names, one function per line. The content of this file must be managed manually because the order
// of existing functions must remain the same so old mpy files can continue to be linked correctly,
// i.e. new functions should always be added to the end.
// The port_fun.mk makefile aids in managing the list of potential new functions.
// The functions come from the intersection of the esp-idf api reference documentation and the
// symbols found in the firmware. This process requires that the esp-idf docs be built first (cd
// $ESPIDF/docs/en; make xml).

// declare all the functions we reference as "void foo(void)" so we can build an array
typedef void voidfun(void);
extern voidfun
#include "port_fun.inc"
__notused__;

// Create static array of function pointers.
// This table plus the code it references (that might otherwise be excluded by the linker) consumes
// an extra 65KB of text (~6%) and 16KB of data (~6%).
const mp_port_fun_table_t mp_port_fun_table = {
    #include "port_fun.inc"
};
const size_t mp_port_fun_table_sz = sizeof(mp_port_fun_table) / sizeof(mp_port_fun_ptr_t);

#endif // MICROPY_PORT_FUN_TABLE
