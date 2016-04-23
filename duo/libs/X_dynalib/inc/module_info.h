/**
 ******************************************************************************
 * @file    module_info.h
 * @authors Matthew McGowan
 * @date    17 February 2015
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

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

#ifndef MODULE_INFO_H
#define	MODULE_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "static_assert.h"
#include "stddef.h"
#include <stdint.h>

typedef struct module_dependency_t {
    uint8_t module_function;        // module function, lowest 4 bits
    uint8_t module_index;           // moudle index, lowest 4 bits.
    uint16_t module_version;        // version/release number of the module.
} module_dependency_t;

/**
 * Describes the module info struct placed at the start of
 */
typedef struct module_info_t {
    const void* module_start_address;   /* the first byte of this module in flash */
    const void* module_end_address;     /* the last byte (exclusive) of this smodule in flash. 4 byte crc starts here. */
    uint8_t reserved;
    uint8_t reserved2;
    uint16_t module_version;            /* 16 bit version */
    uint16_t platform_id;               /* The platform this module was compiled for. */
    uint8_t  module_function;           /* The module function */
    uint8_t  module_index;
    module_dependency_t dependency;
    uint32_t reserved3;
} module_info_t;

#define STATIC_ASSERT_MODULE_INFO_OFFSET(field, expected) STATIC_ASSERT( module_info_##field, offsetof(module_info_t, field)==expected || sizeof(void*)!=4)

STATIC_ASSERT_MODULE_INFO_OFFSET(module_start_address, 0);
STATIC_ASSERT_MODULE_INFO_OFFSET(module_end_address, 4);
STATIC_ASSERT_MODULE_INFO_OFFSET(reserved, 8);
STATIC_ASSERT_MODULE_INFO_OFFSET(reserved2, 9);
STATIC_ASSERT_MODULE_INFO_OFFSET(module_version, 10);
STATIC_ASSERT_MODULE_INFO_OFFSET(platform_id, 12);
STATIC_ASSERT_MODULE_INFO_OFFSET(module_function, 14);
STATIC_ASSERT_MODULE_INFO_OFFSET(module_index, 15);
STATIC_ASSERT_MODULE_INFO_OFFSET(dependency, 16);
STATIC_ASSERT_MODULE_INFO_OFFSET(reserved3, 20);


/**
 * Define the module function enum also as preprocessor symbols so we can
 * set some defaults in the preprocessor.
 */
#define MOD_FUNC_NONE            0
#define MOD_FUNC_RESOURCE        1
#define MOD_FUNC_BOOTLOADER      2
#define MOD_FUNC_MONO_FIRMWARE   3
#define MOD_FUNC_SYSTEM_PART     4
#define MOD_FUNC_USER_PART       5
#define MOD_FUNC_SETTINGS        6

typedef enum module_function_t {
    MODULE_FUNCTION_NONE = MOD_FUNC_NONE,

    /* The module_info and CRC is not part of the resource. */
    MODULE_FUNCTION_RESOURCE = MOD_FUNC_RESOURCE,

    /* The module is the bootloader */
    MODULE_FUNCTION_BOOTLOADER = MOD_FUNC_BOOTLOADER,

    /* The module is complete system and user firmware */
    MODULE_FUNCTION_MONO_FIRMWARE = MOD_FUNC_MONO_FIRMWARE,

    /* The module is a system part */
    MODULE_FUNCTION_SYSTEM_PART = MOD_FUNC_SYSTEM_PART,

    /* The module is a user part */
    MODULE_FUNCTION_USER_PART = MOD_FUNC_USER_PART,

    /* Rewrite persisted settings */
    MODULE_FUNCTION_SETTINGS = MOD_FUNC_SETTINGS
} module_function_t;

typedef enum {

    MODULE_STORE_MAIN = 0,
    /**
     * Factory restore module.
     */
    MODULE_STORE_FACTORY = 1,

    /**
     * An area that saves a copy of modules.
     */
    MODULE_STORE_BACKUP = 2,

    /**
     * Temporary area used to store the module before transferring to it's
     * target.
     */
    MODULE_STORE_SCRATCHPAD = 3,

} module_store_t;


/**
 * Fetches the module function for the given module.
 */
module_function_t  module_function(const module_info_t* mi);
module_store_t module_store(const module_info_t* mi);
uint32_t module_length(const module_info_t* mi);
uint8_t module_index(const module_info_t* mi);

uint16_t module_platform_id(const module_info_t* mi);

uint8_t module_funcion_store(module_function_t function, module_store_t store);

typedef enum module_scheme_t {
    MODULE_SCHEME_MONO,                           /* monolithic firmware */
    MODULE_SCHEME_SPLIT,                          /* modular - called it split to make the names more distinct and less confusable. */
} module_scheme_t;

module_scheme_t module_scheme(const module_info_t* mi);

/**
 * Verifies the module platform ID matches the current system platform ID.
 * @param mi
 * @return
 */
uint8_t module_info_matches_platform(const module_info_t* mi);

/*
 * The structure is a suffix to the module, placed before the end symbol
 */
typedef struct module_info_suffix_t {
    // NB: NB: NB: add new members here
    uint16_t reserved;
    uint8_t sha[32];
    uint16_t size;
    // NB: NB: NB: add new members to the start of this module definition, not the end!!
} __attribute__((packed)) module_info_suffix_t;

/**
 * The structure appended to the end of the module.
 */
typedef struct module_info_crc_t {
    uint32_t crc32;
} __attribute__((packed)) module_info_crc_t;

extern const module_info_t module_info;
extern const module_info_suffix_t module_info_suffix;
extern const module_info_crc_t module_info_crc;


#ifdef __cplusplus
}
#endif


#endif	/* MODULE_INFO_H */

