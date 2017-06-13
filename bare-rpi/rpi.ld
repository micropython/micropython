SECTIONS {
	.init 0x8000 : { *(.init) }
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss*) }
	.rodata : {	*(.rodata) }
	/DISCARD/ : {
	    //*(*)
		*(.ARM.exidx*)
	}
	.common : { *(*) }
	. = ALIGN(8);
    . = . + 0x1000;
    irq_stack_top = .;
    . = . + 0x1000;
    fiq_stack_top = .;
    . = . + 0x1000;
    heap_end = .;
    . = . + 0xf00000;
    stack_top = .;
}
