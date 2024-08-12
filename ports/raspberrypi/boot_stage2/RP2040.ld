MEMORY {
    /* We are loaded to the top 256 bytes of SRAM, which is above the bootrom
       stack. Note 4 bytes occupied by checksum. */
    SRAM(rx) : ORIGIN = 0x20041f00, LENGTH = 252
}

SECTIONS {
    . = ORIGIN(SRAM);
    .text : {
        *(.entry.*)
        *(.text.*)
    } >SRAM
}
