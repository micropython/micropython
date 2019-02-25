MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 32K
}

SECTIONS
{
    .header :
    {
        KEEP(header*(.rodata*))
    } >FLASH

    .text :
    {
        KEEP(*(.text*))
        KEEP(*(.rodata*))
    } >FLASH

    .trailer :
    {
        . = ALIGN(256);
        KEEP(trailer*(.rodata*))
    } >FLASH
}
