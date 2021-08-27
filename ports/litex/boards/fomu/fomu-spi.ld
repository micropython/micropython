/*
    GNU linker script for Fomu
*/

ENTRY(_start)

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)               : ORIGIN = 0x20040000, LENGTH = 0x100000 /* entire flash, 1 MiB */
    FLASH_FIRMWARE (rx)      : ORIGIN = 0x20040000, LENGTH = 0x100000 /* entire flash, 1 MiB */
    RAM (xrw)                : ORIGIN = 0x10000000, LENGTH = 0x00020000 /* 128 KiB */
}

/* top end of the stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* define output sections */
SECTIONS
{
    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH_FIRMWARE (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data :  AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start; used by startup code in order to initialise the .data section in RAM */

        *(.itcm.*)         /* Instruction Tightly Coupled Memory */
        *(.dtcm_data.*)    /* Data Tightly Coupled Memory */

        *(.text.cmp_lfn)
        *(.text.qstr_find_strn)
        *(.text.dcd_edpt_xfer)
        *(.text.pop_rule)
        *(.text.ff_wtoupper)
        *(.text.dir_find)
        *(.text.push_rule)
        *(.text.csr_writel)
        *(.text.csr_readl)
        *(.text.timer0_ev_pending_write)
        *(.text.autoreload_tick)
        *(.text.filesystem_tick)
        *(.text.usb_background)

        *(.text.dcd_*)
        *(.text.tud_control_*)
        *(.text.tud_cdc_n_write_flush)
        *(.text.tud_task)
        *(.text.tu_edpt_dir)
        *(.text.tu_fifo_empty)
        *(.text.usbd_edpt_busy)
        *(.text.usb_irq_handler)
        *(.text.supervisor_tick)
        *(.text.port_get_raw_ticks)
        *(.text.__modsi3)
        *(.text.__udivsi3)
        *(.text.irq_getmask)
        *(.text.irq_setmask)
        *(.text.irq_pending)
        *(.text._osal_q_lock)
        *(.text.osal_queue_receive)

        *(.text.mp_obj_get_type)
        *(.text.mp_parse)
        *(.text.parse_compile_execute)
        *(.text.mp_map_lookup)
        *(.text.mp_execute_bytecode) /* Note: this function is 7kb */

        *(.ramtext)        /* .text* sections (code) */
        *(.ramtext*)       /* .text* sections (code) */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        *(.sdata)          /* .data sections */
        *(.sdata*)         /* .data* sections */

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end; used by startup code in order to initialise the .data section in RAM */
    } >RAM

    /* The program code and other data goes into FLASH_FIRMWARE */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.text.start)) /* isr vector table */
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        *(.srodata)        /* .rodata sections (constants, strings, etc.) */
        *(.srodata*)       /* .rodata* sections (constants, strings, etc.) */

        . = ALIGN(4);
        _etext = .;        /* define a global symbol at end of code */
        _sidata = _etext;  /* This is used by the startup in order to initialize the .data secion */
    } >FLASH_FIRMWARE

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start; used by startup code */
        *(.bss)
        *(.bss*)
        *(.dtcm_bss.*)     /* Data Tightly Coupled Memory */
        *(.sbss)
        *(.sbss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end; used by startup code */
    } >RAM


    /* this is to define the start of the heap, and make sure we have a minimum size */
    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;    /* define a global symbol at heap start */
    } >RAM
}
