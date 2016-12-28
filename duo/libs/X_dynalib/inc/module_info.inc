/**
 ******************************************************************************
 * @file    module_info.c
 * @authors Matthew McGowan
 * @date    16-February-2015
 * @brief
 ******************************************************************************
 Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation, either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#include "module_info.h"

#if PLATFORM_ID==3
#define _ARM_SECTION(x)
char link_module_start;
char link_module_end;
#else
#define _ARM_SECTION(x) ,x
#endif


module_function_t module_function_(uint8_t f) {
    return (module_function_t)f;
}

module_function_t  module_function(const module_info_t* mi) {
    return mi ? module_function_(mi->module_function) : MODULE_FUNCTION_NONE;
}

uint8_t module_index_(uint8_t value) {
    return value;
}

uint8_t module_index(const module_info_t* mi) {
    return mi ? module_index_(mi->module_index) : 0xFF;
}

uint16_t module_platform_id(const module_info_t* mi) {
    return mi ? mi->platform_id : 0xFFFF;
}

uint32_t module_length(const module_info_t* mi) {
    return mi->module_end_address-mi->module_start_address;
}

module_scheme_t module_scheme(const module_info_t* mi) {
    switch (module_function(mi)) {
        case MODULE_FUNCTION_BOOTLOADER:
        case MODULE_FUNCTION_MONO_FIRMWARE:
            return MODULE_SCHEME_MONO;
    }
    return MODULE_SCHEME_SPLIT;
}

/**
 * Verifies the module platform ID matches the current system platform ID.
 * @param mi
 * @return
 */
uint8_t module_info_matches_platform(const module_info_t* mi) {
    return module_platform_id(mi) == PLATFORM_ID;
}



/**
 * Defines the module info block that appears at the start of the module, or after the VTOR table
 * in modules that have that.
 */

extern char link_module_start;
extern char link_module_end;

#ifndef MODULE_VERSION
    #error MODULE_VERSION not defined
#endif

#ifndef MODULE_FUNCTION
#error MODULE_FUNCTION not defined
#endif

#if MODULE_FUNCTION == MOD_FUNC_BOOTLOADER || MODULE_FUNCTION == MOD_FUNC_MONO_FIRMWARE
    #if !defined(MODULE_INDEX)
        #define MODULE_INDEX 0
    #elif MODULE_INDEX != 0
        #error MODULE_INDEX must be 0 for bootloader and monolithic firmware.
    #endif
#endif

#ifndef MODULE_DEPENDENCY
#error MODULE_DEPENDENCY not defined
#endif

#define MODULE_DEPENDENCY2 0,0,0

#if PLATFORM_ID!=3
__attribute__((externally_visible _ARM_SECTION(section(".modinfo.module_info"))))

const module_info_t module_info = {
    &link_module_start,         /* start address */
    &link_module_end,           /* end address */
    0, 0,                       /* reserved, reserved2 */
    MODULE_VERSION,             /* module version */
    (uint16_t)(PLATFORM_ID),    /* platform ID */
    (uint8_t)(MODULE_FUNCTION), /* module function */
    (uint8_t)(MODULE_INDEX),    /* module index (part1 part2 etc..) */
    { MODULE_DEPENDENCY },
    0
};


/**
 * 4 additional bytes for the product_id and product_version values.
 */
#define MODULE_INFO_SUFFIX_EXTRA 4

// These are placed in a non-default (.rodata) section because they come after all the .rodata is included in the linker scripts.
// Doing it like this is simpler than setting up exclusion rules.

__attribute__((externally_visible _ARM_SECTION(section(".modinfo.module_info_suffix")))) const module_info_suffix_t module_info_suffix = {
    0, { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 }, sizeof(module_info_suffix_t)+MODULE_INFO_SUFFIX_EXTRA
};

__attribute__((externally_visible _ARM_SECTION(section(".modinfo.module_info_crc")))) const module_info_crc_t module_info_crc = {
    0x12345678
};

#endif