

ENTRY(PreProcessForVendor)

INCLUDE "export-rom.txt"


MEMORY
{
  TCM (rwx) : ORIGIN = 0x1FFF0000, LENGTH = 65536
  BD_RAM (rwx)  : ORIGIN = 0x10000bc8, LENGTH = 455735
  SDRAM_RAM (rwx)  : ORIGIN = 0x30000000, LENGTH = 2M
}

STACK_SIZE = 0x2000 ;


SECTIONS
{
  __rom_bss_start__ = 0x10000300;
  __rom_bss_end__ = 0x10000bc8;

  .ram.start.table :
  {
    __ram_image1_text_start__ = .;
    __ram_start_table_start__ = .;
    KEEP(*(SORT(.start.ram.data*)))
    __ram_start_table_end__ = .;
    
  } > BD_RAM


/*  Add . to assign the start address of the section,                                     * 
  *  to prevent the change of the start address by ld doing section alignment */  
  .ram_image1.text . :
  {
  	/* these 4 sections is used by ROM global variable */
  	/* Don't move them and never add RAM code variable to these sections */
    __image1_validate_code__ = .;
    KEEP(*(.image1.validate.rodata*))
    KEEP(*(.infra.ram.data*))
    KEEP(*(.timer.ram.data*))
    KEEP(*(.hal.ram.data*))
    __image1_bss_start__ = .;
    *(.hal.flash.data*)    
    *(.hal.sdrc.data*)    
    __image1_bss_end__ = .;
    
    *(.hal.ram.text*)    
    *(.hal.flash.text*)    
    *(.hal.sdrc.text*)    
    *(.rodata*)  
    *(.infra.ram.text*)
    __ram_image1_text_end__ = .;
  } > BD_RAM

  .image2.start.table :
  {
    __ram_image2_text_start__ = .;
    __image2_entry_func__ = .;
    KEEP(*(SORT(.image2.ram.data*)))
    __image2_validate_code__ = .;
    KEEP(*(.image2.validate.rodata*))    
  } > BD_RAM

  .ram_image2.text :
  {
     KEEP(*(.vectors .vectors.*))
    *(.infra.ram.start*)
    *(.mon.ram.text*)
    *(.text .text.* .gnu.linkonce.t.*)
    *(.ramfunc .ramfunc.*);
    *(.glue_7t) *(.glue_7)
    *(.gnu.linkonce.r.*)


  } > BD_RAM

    .ARM.extab : 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > BD_RAM

	.ram.data :
    {
        __data_start__ = .;
        . = ALIGN(4);
        *(vtable)

        . = ALIGN(4);
        *(.data .data.*);
        

        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);


        /* init data */

        . = ALIGN(4);
        KEEP(*(.init))

        . = ALIGN(4);
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);

        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP(*crtbegin?.o(.ctors))
        KEEP(*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        KEEP(*(.ctors))


        /* finit data */
        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);

        . = ALIGN(4);
        KEEP (*crtbegin.o(.dtors))
        KEEP(*crtbegin?.o(.dtors))
        KEEP(*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*crtend.o(.dtors))
        KEEP(*(.dtors))        

    } > BD_RAM

     . = ALIGN(4);
    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx : AT(__exidx_start)
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)

        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
	 __ram_image2_text_end__ = .;    

    } > BD_RAM
    PROVIDE_HIDDEN (__exidx_end = .);


  .sdr_data : 
  {
    __sdram_data_start__ = .;
    *(.sdram.data*)
    __sdram_data_end__ = .;
    
  } > SDRAM_RAM


  
    /* .bss section which is used for uninitialized data */
    .bss  (NOLOAD):
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = . ;
        _szero = .;
        
        *(.bss .bss.*)
        *(COMMON)
	 *(.bdsram.data*)
        
        . = ALIGN(4);
        _ebss = . ;
        _ezero = .;
        __bss_end__ = .;    
    } > BD_RAM

	.bf_data (NOLOAD): 
	{
	__buffer_data_start__ = .;
	*(.bfsram.data*)
	__buffer_data_end__ = .;

	} > BD_RAM

	 .heap (NOLOAD):
	 {
		__end__ = .;
		end = __end__;
		*(.heap*)
		__HeapLimit = .;
	 } > BD_RAM


    /* stack section */
    . = ALIGN(8);
    PROVIDE( _sstack = .);
    PROVIDE(__StackLimit = _sstack);


    .stack_dummy (NOLOAD) : AT(_sstack)
    {
        . = ALIGN(8);
        . = . + STACK_SIZE;
        . = ALIGN(8);
    } > BD_RAM

        PROVIDE(_estack = .);
	PROVIDE(__StackTop = _estack);
	PROVIDE(__stack = __StackTop);



    . = ALIGN(4);
    _end = . ;


}


