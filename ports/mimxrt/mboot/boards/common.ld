/*
** ###################################################################
** Linker script inspired by NXP linker script for MIMXRT10xx
**
** Copyright for original linker script:
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
** Integrated ideas from CircuitPython:
** SPDX-License-Identifier: The MIT License (MIT)
** SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft (tannewt)
**
** Copyright (c) 2021 Philipp Ebensberger
**
** ###################################################################
*/

#include BOARD_LINKER_SCRIPT_H

/* Entry Point */
ENTRY(Reset_Handler)

#if defined MICROPY_HW_FLASH_RESERVED
  /* Reserved Area
   * Users can create a reserved area at the end of the flash memory via
   * 'reserved_size' variable. The size of the reserved area should be a multiple
   * of the sector size of the flash memory!
   */
  reserved_size = MICROPY_HW_FLASH_RESERVED;
#endif

flash_end = DEFINED(reserved_size) ? ((flash_start) + (flash_size - reserved_size)) : ((flash_start) + (flash_size));

/* Flash configuration is physically used only in non-bootloader builds.
 *  For bootloader builds this section will be NOLOAD since information is already present in flash memory through bootloader binary.
 */
flash_config_start      = DEFINED(flash_config_start) ? flash_config_start : flash_start;
flash_config_size       = DEFINED(flash_config_size)  ? flash_config_size  : 0x00001000;

#if MICROPY_HW_FLASH_TYPE == qspi_nor_flash
  firmware_start        = flash_start + MICROPY_HW_FLASH_FIRMWARE_START_ADDR;
#elif MICROPY_HW_FLASH_TYPE == qspi_hyper_flash
  firmware_start        = flash_start + MICROPY_HW_FLASH_FIRMWARE_START_ADDR;
#else
  #error Unknown MICROPY_HW_FLASH_TYPE
#endif

ivt_start             = flash_config_start + flash_config_size;
ivt_size              = 0x00000400;
interrupts_start      = ivt_start + ivt_size;
interrupts_size       = 0x00000400;
text_start            = interrupts_start + interrupts_size;
text_size             = (firmware_start - text_start);

firmware_size           = flash_end - firmware_start;
mboot_flash_start  = flash_start;
mboot_flash_size   = firmware_start - mboot_flash_start;

_estack = __StackTop;
_sstack = __StackLimit;


HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x0400;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x0400;

/* Specify the memory areas */
MEMORY
{
  m_flash_config (RX) : ORIGIN = flash_config_start,    LENGTH = flash_config_size
  m_ivt          (RX) : ORIGIN = ivt_start,             LENGTH = ivt_size
  m_interrupts   (RX) : ORIGIN = interrupts_start,      LENGTH = interrupts_size
  m_text         (RX) : ORIGIN = text_start,            LENGTH = text_size
  m_firmware     (RX) : ORIGIN = firmware_start,        LENGTH = firmware_size
  m_itcm         (RX) : ORIGIN = itcm_start,            LENGTH = itcm_size
  m_dtcm         (RW) : ORIGIN = dtcm_start,            LENGTH = dtcm_size
  m_ocrm         (RW) : ORIGIN = ocrm_start,            LENGTH = ocrm_size
}

/* Define output sections */
SECTIONS
{
  __flash_start = flash_start;
  __flash_config_start = flash_config_start;
  __firmware_start = ORIGIN(m_firmware);
  __firmware_end = __firmware_start + LENGTH(m_firmware);

  .flash_config :
  {
    FILL(0x00);
    . = ALIGN(4);
    KEEP(* (.boot_hdr.conf))     /* flash config section */
    . = ORIGIN(m_flash_config) + LENGTH(m_flash_config) - 1;
    BYTE(0x00)
    . = ALIGN(4);
  } > m_flash_config

  __FLASH_BASE = flash_config_start;
  ivt_begin = ORIGIN(m_flash_config) + LENGTH(m_flash_config);
  .ivt : AT(ivt_begin)
  {
    FILL(0x00);
    __ivt_begin = .;
    . = ALIGN(4);
    KEEP(* (.boot_hdr.ivt))           /* ivt section */
    KEEP(* (.boot_hdr.boot_data))     /* boot section */
    KEEP(* (.boot_hdr.dcd_data))      /* dcd section */
    . = ORIGIN(m_ivt) + LENGTH(m_ivt) - 1;
    BYTE(0x00)
    . = ALIGN(4);
    __ivt_end = .;
  } > m_ivt

  __interrupts_begin = ivt_begin + LENGTH(m_ivt);
  .interrupts : AT(__interrupts_begin)
  {
    __interrupts_begin = .;
    __VECTOR_TABLE = .;
    . = ALIGN(4);
    KEEP(*(.isr_vector))     /* Startup code */
    . = ALIGN(4);
    __interrupts_rom_end = .;
  } > m_interrupts

  __VECTOR_RAM = __VECTOR_TABLE;
  __RAM_VECTOR_TABLE_SIZE_BYTES = 0x0;

  __text_begin = __interrupts_begin + (__interrupts_rom_end - __interrupts_begin);
  _stext = __text_begin;
  .text : AT(__text_begin)
  {
    __text_begin = .;
    . = ALIGN(4);
    *(EXCLUDE_FILE(*fsl_flexspi.o) .text*)
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)               /* glue arm to thumb code */
    *(.glue_7t)              /* glue thumb to arm code */
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    __text_rom_end = .;
  } > m_text

  __extab_begin = __text_begin + (__text_rom_end - __text_begin);
  .ARM.extab : AT(__extab_begin)
  {
    __extab_begin = .;
    PROVIDE_HIDDEN (__arm_extab_start = .);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    PROVIDE_HIDDEN (__arm_extab_end = .);
    __extab_rom_end = .;
  } > m_text

  __arm_begin = __extab_begin + (__extab_rom_end - __extab_begin);
  .ARM : AT(__arm_begin)
  {
    __exidx_begin = .;
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    __exidx_rom_end = .;
  } > m_text

  __preinit_begin = __arm_begin + (__exidx_rom_end - __exidx_begin);
  .preinit_array : AT(__preinit_begin)
  {
    __preinit_array_begin = .;
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    __preinit_array_rom_end = .;
  } > m_text

  __init_array_begin = __preinit_begin + (__preinit_array_rom_end - __preinit_array_begin);
  .init_array : AT(__init_array_begin)
  {
    __init_array_begin = .;
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    __init_array_rom_end = .;
  } > m_text

  __fini_array_begin = __init_array_begin + (__init_array_rom_end - __init_array_begin);
  .fini_array : AT(__fini_array_begin)
  {
    __fini_array_begin = .;
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    __fini_array_rom_end = .;
  } > m_text

  __etext = __fini_array_begin + (__fini_array_rom_end - __fini_array_begin);

  .no_init : AT(__etext)
  {
    __no_init_begin = .;
    KEEP(*(._bl_command*))
    __no_init_rom_end = .;
  } > m_dtcm

  . = __etext + (__no_init_rom_end - __no_init_begin);

  __DATA_ROM = .;

  .data : AT(__DATA_ROM)
  {
    . = ALIGN(4);
    __DATA_RAM = .;
    __data_start__ = .;      /* create a global symbol at data start */
    *(m_usb_dma_init_data)
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > m_dtcm

  __RAM_FUNCTIONS_ROM = __DATA_ROM + (__data_end__ - __data_start__);
  .ram_functions : AT(__RAM_FUNCTIONS_ROM)
  {
    . = ALIGN(4);
    __ram_function_start__ = .;
    *(.ram_functions*)
    /* remaining .text and .rodata; i.e. stuff we exclude above because we want it in RAM */
    *(.text*)
    . = ALIGN(4);
    __ram_function_end__ = .;
  } > m_itcm

  __NDATA_ROM = __DATA_ROM + (__ram_function_end__ - __ram_function_start__);
  .ncache.init : AT(__NDATA_ROM)
  {
    __noncachedata_start__ = .;   /* create a global symbol at ncache data start */
    *(NonCacheable.init)
    . = ALIGN(4);
    __noncachedata_init_end__ = .;   /* create a global symbol at initialized ncache data end */
  } > m_dtcm
  . = __noncachedata_init_end__;
  .ncache :
  {
    *(NonCacheable)
    . = ALIGN(4);
    __noncachedata_end__ = .;     /* define a global symbol at ncache data end */
  } > m_dtcm

  __DATA_END = __NDATA_ROM + (__noncachedata_end__ - __noncachedata_start__);
  text_end = ORIGIN(m_text) + LENGTH(m_text);
  ASSERT(__DATA_END <= (mboot_flash_start + mboot_flash_size), "region m_text overflowed with text and data")

  /* Uninitialized data section */
  .bss (NOLOAD):
  {
    /* This is used by the startup in order to initialize the .bss section */
    . = ALIGN(4);
    __START_BSS = .;
    __bss_start__ = .;
    __bss_section_table = .;
    *(m_usb_dma_noninit_data)
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
    __END_BSS = .;
  } > m_dtcm

  .heap (NOLOAD):
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    __HeapBase = .;
    . += HEAP_SIZE;
    __HeapLimit = .;
    __heap_limit = .; /* Add for _sbrk */
  } > m_dtcm

  .stack :
  {
    . = ALIGN(8);
    . += STACK_SIZE;
  } > m_dtcm

  .fw_buffer (NOLOAD):
  {
    . = ALIGN(0x400);
    *(.fw_buffer);
    . = ALIGN(4);
    __END_FW_BUFFER = .;
  } > m_ocrm

  /* Initializes stack on the end of block */
  __StackTop   = ORIGIN(m_dtcm) + LENGTH(m_dtcm);
  __StackLimit = __StackTop - STACK_SIZE;
  PROVIDE(__stack = __StackTop);


  boot_data_start_address = flash_config_start;
  boot_data_size          = __etext - flash_start;

  .ARM.attributes 0 : { *(.ARM.attributes) }

  ASSERT(__StackLimit >= __HeapLimit, "region m_dtcm overflowed with stack and heap")
}

