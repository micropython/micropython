/* STM32F412RG Memory Layout */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K  /* 1 MB Flash */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 256K   /* 256 KB RAM */
}

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text)        /* Code */
        *(.text.*)
        *(.rodata)      /* Read-only data */
        *(.rodata.*)
    } > FLASH

    .data :
    {
        *(.data)        /* Initialized data */
        *(.data.*)
    } > RAM AT> FLASH

    .bss :
    {
        *(.bss)         /* Uninitialized data */
        *(.bss.*)
    } > RAM

    /* Other sections can be added here as needed */
}
