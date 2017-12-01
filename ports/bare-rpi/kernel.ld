SECTIONS {
    . = 0x8000;
    .init . : { *(.init) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss COMMON) }
	. = ALIGN(8);
    . = . + 0x1000;
    heap_start = .;
    . = . + 0x800000;
    heap_end = .;
    . = . + 0x800000;
    stack_top = .;
}
