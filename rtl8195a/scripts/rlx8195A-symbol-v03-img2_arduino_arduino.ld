ENTRY(Reset_Handler)

INCLUDE "export-rom_v03.txt"

__stack_size__    = 5K;       /* interrupts are handled within this stack */
__min_heap_size__ = 20K;

MEMORY
{
  TCM (rwx) : ORIGIN = 0x1FFF0000, LENGTH = 65536
  ROM_USED_RAM (rwx)  : ORIGIN = 0x10000bc8, LENGTH = 13368
  RECY_RAM (rwx)  : ORIGIN = 0x10002114, LENGTH = 16108
  BD_RAM (rwx)  : ORIGIN = 0x10006000, LENGTH = 434176
  SDRAM_RAM (rwx)  : ORIGIN = 0x30000000, LENGTH = 2M
}

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
    
  } > ROM_USED_RAM


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
    KEEP(*(.cutb.ram.data*))
    KEEP(*(.cutc.ram.data*))
    KEEP(*(.hal.ram.data*))
    .ram_image1.bss$$Base = .;
    __image1_bss_start__ = .;
    .ram_image1.bss$$Limit = .;
    __image1_bss_end__ = .;
    __ram_image1_data_end__ = .;
    
    *(.hal.ram.text*)    
    *(.infra.ram.text*)
    __ram_image1_text_end__ = .;
  } > ROM_USED_RAM

  .tcm :
  {
    __tcm_start__ = .;
    *(.tcm.heap)
    *mem.o (.bss)
    *memp.o (.bss)
    __tcm_end__ = .;
  } > TCM

  .sdr_data : 
  {
    __sdram_data_start__ = .;
    *(.sdram.data*)
    *(.mon.ram.text*)
    INCLUDE "sdram_obj_list.txt"
    __sdram_data_end__ = .;

    /* define micropython heap start and end */
    . = ALIGN(8);
    _mp_heap_head = .;
    . = . + __min_heap_size__;
    . = . + (ORIGIN(SDRAM_RAM) + LENGTH(SDRAM_RAM) - __stack_size__ - ABSOLUTE(.));
    . = ALIGN(8);
    _mp_heap_end = .;

  } > SDRAM_RAM

  .image2.start.table :
  {
    .image2.start.table1$$Base = .;
    __ram_image2_text_start__ = .;
    __image2_entry_func__ = .;
    KEEP(*(SORT(.image2.ram.data*)))
    __image2_validate_code__ = .;
    KEEP(*(.image2.validate.rodata*))
    
  } > BD_RAM

  .ram_image2.text :
  {
    KEEP(*(.infra.ram.start*))

    PROVIDE (_init = .);

    . = ALIGN(4);
    KEEP(*(.init))

    /* init data */
    . = ALIGN(4);
    PROVIDE (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE (__init_array_end = .);

    . = ALIGN(4);
    KEEP(*(.fini))

    . = ALIGN(4);
    PROVIDE (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE (__fini_array_end = .);

    *(.rodata*)
    *(.hal.flash.text*)    
    *(.hal.sdrc.text*)
    *(.hal.gpio.text*)
    *(.fwu.rodata*)
    *(.fwu.text*)
    *(.text*)
  } > BD_RAM

  .ram.data : 
  {
    __data_start__ = .;
    *(.data*)
    __data_end__ = .;
  	__ram_image2_text_end__ = .;
  } > BD_RAM
  
  .ram.bss :
  {
    .ram.bss$$Base = .;
    __bss_start__ = .;
    *(.hal.flash.data*)    
    *(.hal.sdrc.data*)    
    *(.hal.gpio.data*)
    *(.fwu.data*)
    *(.bss*)
    *(.mon.ram.bss*)
    *(COMMON)
    *(.bdsram.data*)
    __bss_end__ = .;
    .ram.bss$$Limit = .;
  } > BD_RAM

  .bf_data : 
  {
    __buffer_data_start__ = .;
    *(.bfsram.data*)
    __buffer_data_end__ = .;
    
  } > BD_RAM

  .bf_data2 : 
  {
    __buffer_data_start2__ = .;
    __buffer_data_end2__ = .;
    
  } > RECY_RAM

 .heap :
 {
	__end__ = .;
	end = __end__;
	*(.heap*)
	__HeapLimit = .;
 } > BD_RAM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy :
	{
		*(.stack)
	} > BD_RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(BD_RAM) + LENGTH(BD_RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

}


