/* Template for iMX RT 10xx linking. This is the last of four linker scripts passed in.

The first three provide variables for this one.

Boards can setup reserved flash with _ld_reserved_flash_size in board.ld. */

ENTRY(Reset_Handler)

code_size = _ld_flash_size >= 4M ? 2M : 1M;
_ld_default_stack_size = 20K;

/* Default reserved flash to nothing. */
_ld_reserved_flash_size = DEFINED(_ld_reserved_flash_size) ? _ld_reserved_flash_size : 0K ;

MEMORY
{
    /* These next two sections are included in place of a bootloader. If a UF2 is used to load, it
      will ignore these two sections because it lives there. */
    /* This is the first block and is read so that the bootrom knows the optimal way to interface with the flash chip. */
    FLASH_CONFIG (rx)     : ORIGIN = flash_config_location, LENGTH = 512
    /* This can't move because the bootrom looks at this address. */
    FLASH_IVT (rx)        : ORIGIN = 0x60001000, LENGTH = 4K
    /* Place the ISRs 48k in to leave room for the bootloader when it is available. */
    FLASH_FIRMWARE (rx)   : ORIGIN = 0x6000C000, LENGTH = code_size - 48K
    FLASH_FATFS (r)       : ORIGIN = 0x60000000 + code_size, LENGTH = _ld_flash_size - code_size - _ld_reserved_flash_size
    /* Teensy uses the last bit of flash for recovery. */
    RESERVED_FLASH        : ORIGIN = 0x60000000 + code_size + _ld_flash_size - _ld_reserved_flash_size, LENGTH = _ld_reserved_flash_size
    OCRAM (rwx)           : ORIGIN = 0x20200000, LENGTH = ram_size - 64K
    DTCM (x)              : ORIGIN = 0x20000000, LENGTH = 32K
    ITCM (x)              : ORIGIN = 0x00000000, LENGTH = 32K
}

__data_start__ = 0;
__data_end__ = 0;
_start = 0;

SECTIONS
{
    .flash_config :
    {
        . = ALIGN(4);
        KEEP(* (.boot_hdr.conf))
        . = ALIGN(4);
    } > FLASH_CONFIG

    .ivt :
    {
        . = ALIGN(4);
        KEEP(* (.boot_hdr.ivt))
        KEEP(* (.boot_hdr.boot_data))
        KEEP(* (.boot_hdr.dcd_data))
        . = ALIGN(4);
    } > FLASH_IVT

    /* Align for 256 ISR entries and place first in flash. Otherwise the UF2
       bootloader can't find it because it uses its own flash_config and ivt. */
    .isr_vector : ALIGN(4 * 256)
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))         /* Startup code */
        . = ALIGN(4);
    } > ITCM AT> FLASH_FIRMWARE
    _ld_isr_destination = ADDR(.isr_vector);
    _ld_isr_flash_copy = LOADADDR(.isr_vector);
    _ld_isr_size = SIZEOF(.isr_vector);
    /* Used by the bootloader to start user code. */
    __VECTOR_TABLE = LOADADDR(.isr_vector);

    .text :
    {
        . = ALIGN(4);
    *(EXCLUDE_FILE(
        *fsl_flexspi.o
        *dcd_ci_hs.o
        *tusb_fifo.o
        *usbd.o
        *string0.o
        *py/nlr*.o
        *py/obj.o
        *py/gc.o
        *py/map.o
        *py/runtime.o
        *py/objboundmeth.o
        *py/objtype.o
    ) .text*)                /* .text* sections (code) */

        /* Keep USB processing functions out of RAM because we don't know which will be used.
           We try to only keep USB interrupt related functions. */
        *dcd_ci_hs.o(.text.process_*_request .text.dcd_edpt* .text.dcd_init .text.dcd_set_address)
        *usbd.o(.text.process_*_request .text.process_[gs]et* .text.tud_* .text.usbd_* .text.configuration_reset .text.invoke_*)

        /* Anything marked cold/unlikely should be in flash. */
        *(.text.unlikely.*)

    *(EXCLUDE_FILE(
        *dcd_ci_hs.o
        *py/objboundmeth.o
        *py/objtype.o
    ) .rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } > FLASH_FIRMWARE

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        *(.gnu.linkonce.armexidx.*)
        _etext = .;        /* define a global symbol at end of code */
        __etext = .;       /* define a global symbol at end of code */
    } > FLASH_FIRMWARE
    _ld_filesystem_start = ORIGIN(FLASH_FATFS);
    _ld_filesystem_end = _ld_filesystem_start + LENGTH(FLASH_FATFS);

    .data :
    {
        . = ALIGN(4);
        *(.data*)               /* .data* sections */
        . = ALIGN(4);
    } > OCRAM AT> FLASH_FIRMWARE
    _ld_ocram_data_destination = ADDR(.data);
    _ld_ocram_data_flash_copy = LOADADDR(.data);
    _ld_ocram_data_size = SIZEOF(.data);

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);

        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss*)))
        *(COMMON)

        . = ALIGN(4);
    } > OCRAM
    _ld_ocram_bss_start = ADDR(.bss);
    _ld_ocram_bss_size = SIZEOF(.bss);
    _ld_heap_start = _ld_ocram_bss_start + _ld_ocram_bss_size;
    _ld_heap_end = ORIGIN(OCRAM) + LENGTH(OCRAM);


    .itcm : ALIGN(4)
    {
        . = ALIGN(4);
        *(.itcm.*)
        *fsl_flexspi.o(.text*)
        *dcd_ci_hs.o(.text*)
        *tusb_fifo.o(.text*)
        *py/objboundmeth.o(.text*)
        *py/objtype.o(.text*)
        *py/obj.o(.text*)
        *py/gc.o(.text*)
        *py/map.o(.text*)
        *py/nlr*.o(.text*)
        *py/runtime.o(.text*)
        *(.text.process_*_isr .text.dcd_event_* .text.osal_queue*)
        *string0.o(.text*)
        . = ALIGN(4);
    } > ITCM AT> FLASH_FIRMWARE
    _ld_itcm_destination = ADDR(.itcm);
    _ld_itcm_flash_copy = LOADADDR(.itcm);
    _ld_itcm_size = SIZEOF(.itcm);

    .dtcm_data :
    {
        . = ALIGN(4);

        *(.dtcm_data.*)
        *dcd_ci_hs.o(.rodata*)
        *py/objboundmeth.o(.rodata*)
        *py/objtype.o(.rodata*)

        . = ALIGN(4);
    } > DTCM AT> FLASH_FIRMWARE
    _ld_dtcm_data_destination = ADDR(.dtcm_data);
    _ld_dtcm_data_flash_copy = LOADADDR(.dtcm_data);
    _ld_dtcm_data_size = SIZEOF(.dtcm_data);

    _flashimagelen = _ld_dtcm_data_flash_copy + _ld_dtcm_data_size - flash_config_location;

    .dtcm_bss :
    {
        . = ALIGN(4);

        *(.dtcm_bss.*)

        . = ALIGN(4);
    } > DTCM AT> DTCM
    _ld_dtcm_bss_start = ADDR(.dtcm_bss);
    _ld_dtcm_bss_size = SIZEOF(.dtcm_bss);

    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        _ld_stack_bottom = .;
        . += _ld_default_stack_size;
    } > DTCM
    _ld_stack_top = ORIGIN(DTCM) + LENGTH(DTCM);

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
