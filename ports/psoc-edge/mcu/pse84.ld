/* GNU linker script for STM32F405 */

MEMORY
{
    ROM (rx)        : ORIGIN = 0x70100400, LENGTH = 256K
    RAM (xrw)       : ORIGIN = 0x240bd000, LENGTH = 256K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);
STACK_SIZE = 8K;
__MP_HEAP_SIZE = 128K;

SECTIONS
{
    .text :/* AT (0xc8100400)*/
    {
        . = ALIGN(4);
        KEEP(*(.vectors))
        . = ALIGN(4);
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } >ROM

     /* A section for the veneer code of Secure APIs */
    .gnu.sgstubs : ALIGN(32)
    {
        . = ALIGN(32);
        __nsc_start = .;
        KEEP(*(.gnu.sgstubs*))
        . = ALIGN(32);
    } > ROM
        /* Empty section to generate the end address of the NSC section correctly
       see https://answers.launchpad.net/gcc-arm-embedded/+question/697333  */
    empty_section : ALIGN(32)
    {
        __nsc_end = .;
    } > ROM

    PROVIDE(__nsc_region_start = __nsc_start);
    PROVIDE(__nsc_region_size = __nsc_end - __nsc_start);

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
    __zero_table_end__ = .;
    . = ALIGN(16);
  } > ROM

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
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT >ROM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = .;
    } >RAM

  .heap (NOLOAD) : ALIGN(4)
  {
    __end__ = .;
    . = ALIGN(4);

    /* MicroPython GC heap */
    . = ALIGN(16);
    __GcHeapStart = .;
    . = . + __MP_HEAP_SIZE;
    __GcHeapEnd = .;
  } > RAM
}

__StackTop = _estack;
__StackLimit = __StackTop - STACK_SIZE;
__stack = __StackTop; /* for libgcc _mainCRTStartup() */
