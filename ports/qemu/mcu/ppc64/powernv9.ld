SECTIONS
{
    . = 0;
    _start = .;
    .head : {
        KEEP(*(.head))
    }
    /* Put this at 0x1700 which is right after our exception
     * vectors in head.S.
     */
    . = 0x1700;
    .text : {
        *(.text)
        *(.text.*)
        . = ALIGN(4K);
        *(.rodata)
        *(.rodata.*)
    }
    . = ALIGN(4K);
    .data : {
        *(.data)
        *(.data.*)
        . = ALIGN(4K);
        *(.bss)
        *(.bss.*)
    }
    . = ALIGN(4K);
    .stack : {
        PROVIDE(_sstack = .);
        . = . + 128K;
        PROVIDE(_estack = .);
    } = 0x0000000000000000
}

/* Recent powerpc64 toolchain glibc uses stdio bits macros to redefine
 * snprintf to __snprintfieee128 (for -mlong-double-128) or
 * __nldbl_snprintf (for -mlong-double-64). Linking fails as these
 * symbols are in libc.a, not linked on bare metal. We actually
 * want to link against the plain snprintf() function implemented
 * in-tree under shared/libc/printf.c
 */
PROVIDE(__snprintfieee128 = snprintf);

