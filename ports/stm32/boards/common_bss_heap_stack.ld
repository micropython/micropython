/* This linker script fragment is intended to be included in SECTIONS. */

/* Zeroed-out data section */
.bss :
{
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
} >RAM

/* This is to define the start of the heap, and make sure there is a minimum size */
.heap :
{
    . = ALIGN(4);
    . = . + _minimum_heap_size;
    . = ALIGN(4);
} >RAM

/* This checks there is enough RAM for the stack */
.stack :
{
    . = ALIGN(4);
    . = . + _minimum_stack_size;
    . = ALIGN(4);
} >RAM

/* _bl_state symbol is used by MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET, this is
   the end of stack address but is accessed as a different type. */
_bl_state = _estack;
