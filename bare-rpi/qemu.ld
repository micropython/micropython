ENTRY(_start)
SECTIONS
{
    . = 0x10000;
    .init . : { *(.init) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss COMMON) }
     . = ALIGN(8);
     . = . + 0x1000; /* 4kB of stack memory */
    irq_stack_top = .;
    . = . + 0x1000;
    fiq_stack_top = .;
    . = . + 0x1000;
    heap_end = .;
    . = . + 0xf00000;
    stack_top = .;
}
