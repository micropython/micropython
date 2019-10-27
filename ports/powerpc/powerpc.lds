SECTIONS
{
        . = 0;
        _start = .;
        .head : {
                KEEP(*(.head))
        }

        /* Put this at 0x1700 which is right after our execption
         * vectors in head.S.
         */
        . = 0x1700;
}
