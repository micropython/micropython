/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 OpenMV LLC.
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

// Entry Point
ENTRY(Reset_Handler)

MEMORY
{
  ROM   (rx)  : ORIGIN = 0x80000000, LENGTH = 0x0057F000
  ITCM  (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
  #ifdef CORE_M55_HP
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00100000
  #else
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000
  #endif
  SRAM0 (rwx) : ORIGIN = 0x02000000, LENGTH = 0x00400000
  SRAM1 (rwx) : ORIGIN = 0x08000000, LENGTH = 0x00280000
}

__STACK_SIZE = 0x00004000;
__HEAP_SIZE  = 0x00004000;
__MP_HEAP_SIZE = 0x00040000;

SECTIONS
{
  .text : ALIGN(16)
  {
    KEEP(*(.vectors))
    . = ALIGN(4);
    *(.text*)
    . = ALIGN(4);
    *(.rodata*)
    . = ALIGN(16);
  } > ROM

  .copy.table : ALIGN(4)
  {
    __copy_table_start__ = .;
    LONG ( LOADADDR(.data) )
    LONG ( ADDR(.data) )
    LONG ( SIZEOF(.data)/4 )
    __copy_table_end__ = .;
    . = ALIGN(16);
  } > ROM

  .zero.table : ALIGN(4)
  {
    __zero_table_start__ = .;
    LONG (ADDR(.bss))
    LONG (SIZEOF(.bss)/4)
    LONG (ADDR(.bss.sram0))
    LONG (SIZEOF(.bss.sram0)/4)
    __zero_table_end__ = .;
    . = ALIGN(16);
  } > ROM

  .data : ALIGN(8)
  {
    *(.data)
    . = ALIGN(8);
    *(.data.*)
    . = ALIGN(16);
  } > DTCM AT > ROM

  /* Peripherals in expansion master 0 (USB, Ethernet, SD/MMC)
     are by default configured as non-secure, so they don't
     have access to DTCMs. This can be fixed in the ToC by allowing
     access to DTCMs to all bus masters, for now these peripherals
     should place buffers in regular SRAM */
  .bss.sram0 (NOLOAD) : ALIGN(4)
  {
    * (.bss.sram0*)
  } > SRAM0

  .bss : ALIGN(4)
  {
    __bss_start__ = .;
    *(.bss)
    . = ALIGN(4);
    *(.bss.*)
    . = ALIGN(4);
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > DTCM

  .heap (NOLOAD) : ALIGN(4)
  {
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(4);
    __HeapLimit = .;

    /* MicroPython GC heap */
    . = ALIGN(16);
    __GcHeapStart = .;
    . = . + __MP_HEAP_SIZE;
    __GcHeapEnd = .;
  } > DTCM

  .stack (NOLOAD) : ALIGN(4)
  {
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(4);
    __StackTop = .;
  } > DTCM
  PROVIDE(__stack = __StackTop);

  .init_fini_arrays : ALIGN(16)
  {
    KEEP(*(.init))
    KEEP(*(.fini))

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

    KEEP(*(.eh_frame*))
    . = ALIGN(16);
  } > ROM

  /* Check if data + heap + stack exceeds RAM limit */
  ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
