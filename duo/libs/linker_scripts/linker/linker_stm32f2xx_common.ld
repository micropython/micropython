
PROVIDE ( _Stack_Size = __Stack_Size ) ;
/*"PROVIDE" allows to easily override these values from an object file or the commmand line.*/
PROVIDE ( _Stack_Init = __Stack_Init ) ;


/* include the memory spaces definitions sub-script */
/*
Linker subscript for STM32F205xx definitions with 1024K Flash and 128K RAM
*/

/* higher address of the user mode stack */
_estack = 0x20020000;	/* end of 128K SRAM */

/* include the sections management sub-script for FLASH mode */

/* Sections Definitions */

SECTIONS
{
    INCLUDE module_start.ld

    /* for Cortex devices, the beginning of the startup code is stored in the .isr_vector section, which goes to FLASH */
    .isr_vector :
    {
	. = ALIGN(4);
        KEEP(*(.interrupt_vector_table))            /* Startup code */
	. = ALIGN(4);
    } >APP_FLASH

    INCLUDE module_info.ld

    .flashtext :
    {
	. = ALIGN(4);
        *(.flashtext)            /* Startup code */
	. = ALIGN(4);
    } >APP_FLASH

    /* the program code is stored in the .text section, which goes to Flash */
    .text :
    {
	    . = ALIGN(4);

        *(.text)                   /* remaining code */
        *(.text.*)                   /* remaining code */
        *(.rodata)                 /* read-only data (constants) */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)

        link_constructors_location = .;
        chk_system_pre_init_start = .;
        KEEP (*(.module_pre_init))
        chk_system_pre_init_end = .;
        KEEP(*(.preinit_array))
        KEEP(*(.init_array))
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        chk_system_init_start = .;
        KEEP (*(.module_init))
        chk_system_init_end = .;
        link_constructors_end = .;

	    . = ALIGN(4);
   	  _etext = .;
	    /* This is used by the startup in order to initialize the .data secion */

    } >APP_FLASH

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >APP_FLASH
    .ARM : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >APP_FLASH


    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data  :
    {
        _sidata = LOADADDR(.data);
	    . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
        _sdata = . ;

        *(.data)
        *(.data.*)

	    . = ALIGN(4);
	    /* This is used by the startup in order to initialize the .data secion */
   	 _edata = . ;
    } >RAM AT>APP_FLASH

	INCLUDE backup_ram_system.ld
	INCLUDE backup_ram_user.ld

    /* This is the uninitialized data section */
    .bss :
    {
	    . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .;

        *(.bss)
        *(.bss*)
        *(COMMON)

	    . = ALIGN(4);
	    /* This is used by the startup in order to initialize the .bss secion */
   	 _ebss = . ;
    } >RAM

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );

    /* This is the user stack section
    This is just to check that there is enough RAM left for the User mode stack
    It should generate an error if it's full.
     */
    ._usrstack :
    {
	    . = ALIGN(4);
        _susrstack = . ;

        . = . + _Minimum_Stack_Size ;

	    . = ALIGN(4);
        _eusrstack = . ;
    } >RAM

    /* after that it's only debugging information. */

    /* remove the debugging information from the standard libraries */
    DISCARD :
    {
     libc.a ( * )
     libm.a ( * )
     libgcc.a ( * )
     }

    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }

    INCLUDE module_end.ld
}


PROVIDE( dct1_start_addr_loc     = ORIGIN( DCT1_FLASH ));
PROVIDE( dct1_size_loc           = LENGTH( DCT1_FLASH ));
PROVIDE( dct2_start_addr_loc     = ORIGIN( DCT2_FLASH ));
PROVIDE( dct2_size_loc           = LENGTH( DCT2_FLASH ));

ASSERT ( link_module_info_start < link_module_info_end, "module info not linked" );
ASSERT ( link_module_info_start - ORIGIN(APP_FLASH) == 0x184,  "module info not where expected" )
ASSERT ( link_module_info_suffix_start < link_module_info_suffix_end, "module info suffix not linked" );
ASSERT ( link_module_info_crc_start < link_module_info_crc_end, "module info crc not linked" );
