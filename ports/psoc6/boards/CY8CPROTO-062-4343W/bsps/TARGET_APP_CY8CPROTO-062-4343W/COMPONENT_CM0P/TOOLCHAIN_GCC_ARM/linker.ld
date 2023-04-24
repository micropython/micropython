/***************************************************************************//**
* \file cy8c6xxa_cm0plus.ld
* \version 2.95.1
*
* Linker file for the GNU C compiler.
*
* The main purpose of the linker script is to describe how the sections in the
* input files should be mapped into the output file, and to control the memory
* layout of the output file.
*
* \note The entry point location is fixed and starts at 0x10000000. The valid
* application image should be placed there.
*
* \note The linker files included with the PDL template projects must be generic
* and handle all common use cases. Your project may not use every section
* defined in the linker files. In that case you may see warnings during the
* build process. In your project, you can simply comment out or remove the
* relevant code in the linker file.
*
********************************************************************************
* \copyright
* Copyright 2016-2021 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
ENTRY(Reset_Handler)

/* The size of the stack section at the end of CM0+ SRAM */
STACK_SIZE = 0x1000;

/* Force symbol to be entered in the output file as an undefined symbol. Doing
* this may, for example, trigger linking of additional modules from standard
* libraries. You may list several symbols for each EXTERN, and you may use
* EXTERN multiple times. This command has the same effect as the -u command-line
* option.
*/
EXTERN(Reset_Handler)

/* The MEMORY section below describes the location and size of blocks of memory in the target.
* Use this section to specify the memory regions available for allocation.
*/
MEMORY
{
    /* The ram and flash regions control RAM and flash memory allocation for the CM0+ core.
     * You can change the memory allocation by editing the 'ram' and 'flash' regions.
     * Note that 2 KB of RAM (at the end of the SRAM) are reserved for system use.
     * Using this memory region for other purposes will lead to unexpected behavior.
     * Your changes must be aligned with the corresponding memory regions for the CM4 core in 'xx_cm4_dual.ld',
     * where 'xx' is the device group; for example, 'cy8c6xx7_cm4_dual.ld'.
     */
    ram               (rwx)   : ORIGIN = 0x08000000, LENGTH = 0x2000
    flash             (rx)    : ORIGIN = 0x10000000, LENGTH = 0x2000


    /* This is an unprotected public RAM region, with the placed .cy_sharedmem.
     * This region is used to place objects that require full access from both cores.
     * Uncomment the following line, define the region origin and length, and uncomment the placement of
     * the .cy_sharedmem section below.
     */
    /* public_ram        (rw)    : ORIGIN = %REGION_START_ADDRESS%, LENGTH = %REGION_SIZE% */

    /* This is a 32K flash region used for EEPROM emulation. This region can also be used as the general purpose flash.
     * You can assign sections to this memory region for only one of the cores.
     * Note some middleware (e.g. BLE, Emulated EEPROM) can place their data into this memory region.
     * Therefore, repurposing this memory region will prevent such middleware from operation.
     */
    em_eeprom         (rx)    : ORIGIN = 0x14000000, LENGTH = 0x8000       /*  32 KB */

    /* The following regions define device specific memory regions and must not be changed. */
    sflash_user_data  (rx)    : ORIGIN = 0x16000800, LENGTH = 0x800        /* Supervisory flash: User data */
    sflash_nar        (rx)    : ORIGIN = 0x16001A00, LENGTH = 0x200        /* Supervisory flash: Normal Access Restrictions (NAR) */
    sflash_public_key (rx)    : ORIGIN = 0x16005A00, LENGTH = 0xC00        /* Supervisory flash: Public Key */
    sflash_toc_2      (rx)    : ORIGIN = 0x16007C00, LENGTH = 0x200        /* Supervisory flash: Table of Content # 2 */
    sflash_rtoc_2     (rx)    : ORIGIN = 0x16007E00, LENGTH = 0x200        /* Supervisory flash: Table of Content # 2 Copy */
    xip               (rx)    : ORIGIN = 0x18000000, LENGTH = 0x8000000    /* 128 MB */
    efuse             (r)     : ORIGIN = 0x90700000, LENGTH = 0x100000     /*   1 MB */
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 */


SECTIONS
{
    .cy_app_header :
    {
        KEEP(*(.cy_app_header))
    } > flash

    /* Cortex-M0+ application flash area */
    .text :
    {
        . = ALIGN(4);
        __Vectors = . ;
        KEEP(*(.vectors))
        . = ALIGN(4);
        __Vectors_End = .;
        __Vectors_Size = __Vectors_End - __Vectors;
        __end__ = .;

        . = ALIGN(4);
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        /* Read-only code (constants). */
        *(.rodata .rodata.* .constdata .constdata.* .conststring .conststring.*)

        KEEP(*(.eh_frame*))
    } > flash


    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > flash

    __exidx_start = .;

    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > flash
    __exidx_end = .;


    /* To copy multiple ROM to RAM sections,
     * uncomment .copy.table section and,
     * define __STARTUP_COPY_MULTIPLE in startup_psoc6_02_cm0plus.S */
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;

        /* Copy interrupt vectors from flash to RAM */
        LONG (__Vectors)                                    /* From */
        LONG (__ram_vectors_start__)                        /* To   */
        LONG (__Vectors_End - __Vectors)                    /* Size */

        /* Copy data section to RAM */
        LONG (__etext)                                      /* From */
        LONG (__data_start__)                               /* To   */
        LONG (__data_end__ - __data_start__)                /* Size */

        __copy_table_end__ = .;
    } > flash


    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_psoc6_02_cm0plus.S */
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        __zero_table_end__ = .;
    } > flash

    __etext =  . ;


    .ramVectors (NOLOAD) : ALIGN(8)
    {
        __ram_vectors_start__ = .;
        KEEP(*(.ram_vectors))
        __ram_vectors_end__   = .;
    } > ram


    .data __ram_vectors_end__ :
    {
        . = ALIGN(4);
        __data_start__ = .;

        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);

        KEEP(*(.cy_ramfunc*))
        . = ALIGN(4);

        __data_end__ = .;

    } > ram AT>flash


    /* Place variables in the section that should not be initialized during the
    *  device startup.
    */
    .noinit (NOLOAD) : ALIGN(8)
    {
      KEEP(*(.noinit))
    } > ram


    /* The uninitialized global or static variables are placed in this section.
    *
    * The NOLOAD attribute tells linker that .bss section does not consume
    * any space in the image. The NOLOAD attribute changes the .bss type to
    * NOBITS, and that  makes linker to A) not allocate section in memory, and
    * A) put information to clear the section with all zeros during application
    * loading.
    *
    * Without the NOLOAD attribute, the .bss section might get PROGBITS type.
    * This  makes linker to A) allocate zeroed section in memory, and B) copy
    * this section to RAM during application loading.
    */
    .bss (NOLOAD):
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > ram


    .heap (NOLOAD):
    {
        __HeapBase = .;
        __end__ = .;
        end = __end__;
        KEEP(*(.heap*))
        . = ORIGIN(ram) + LENGTH(ram) - STACK_SIZE;
        __HeapLimit = .;
    } > ram


    /* To use unprotected public RAM, uncomment the following .cy_sharedmem section placement.*/
    /*
    .cy_sharedmem (NOLOAD):
    {
        . = ALIGN(4);
        __public_ram_start__ = .;
        KEEP(*(.cy_sharedmem))
        . = ALIGN(4);
        __public_ram_end__ = .;
    } > public_ram
    */

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (NOLOAD):
    {
        KEEP(*(.stack*))
    } > ram


    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(ram) + LENGTH(ram);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")


    /* Emulated EEPROM Flash area */
    .cy_em_eeprom :
    {
        KEEP(*(.cy_em_eeprom))
    } > em_eeprom


    /* Supervisory Flash: User data */
    .cy_sflash_user_data :
    {
        KEEP(*(.cy_sflash_user_data))
    } > sflash_user_data


    /* Supervisory Flash: Normal Access Restrictions (NAR) */
    .cy_sflash_nar :
    {
        KEEP(*(.cy_sflash_nar))
    } > sflash_nar


    /* Supervisory Flash: Public Key */
    .cy_sflash_public_key :
    {
        KEEP(*(.cy_sflash_public_key))
    } > sflash_public_key


    /* Supervisory Flash: Table of Content # 2 */
    .cy_toc_part2 :
    {
        KEEP(*(.cy_toc_part2))
    } > sflash_toc_2


    /* Supervisory Flash: Table of Content # 2 Copy */
    .cy_rtoc_part2 :
    {
        KEEP(*(.cy_rtoc_part2))
    } > sflash_rtoc_2


    /* Places the code in the Execute in Place (XIP) section. See the smif driver
    *  documentation for details.
    */
    cy_xip :
    {
        __cy_xip_start = .;
        KEEP(*(.cy_xip))
        __cy_xip_end = .;
    } > xip


    /* eFuse */
    .cy_efuse :
    {
        KEEP(*(.cy_efuse))
    } > efuse


    /* These sections are used for additional metadata (silicon revision,
    *  Silicon/JTAG ID, etc.) storage.
    */
    .cymeta         0x90500000 : { KEEP(*(.cymeta)) } :NONE
}


/* The following symbols used by the cymcuelftool. */
/* Flash */
__cy_memory_0_start    = 0x10000000;
__cy_memory_0_length   = 0x00200000;
__cy_memory_0_row_size = 0x200;

/* Emulated EEPROM Flash area */
__cy_memory_1_start    = 0x14000000;
__cy_memory_1_length   = 0x8000;
__cy_memory_1_row_size = 0x200;

/* Supervisory Flash */
__cy_memory_2_start    = 0x16000000;
__cy_memory_2_length   = 0x8000;
__cy_memory_2_row_size = 0x200;

/* XIP */
__cy_memory_3_start    = 0x18000000;
__cy_memory_3_length   = 0x08000000;
__cy_memory_3_row_size = 0x200;

/* eFuse */
__cy_memory_4_start    = 0x90700000;
__cy_memory_4_length   = 0x100000;
__cy_memory_4_row_size = 1;

/* EOF */
