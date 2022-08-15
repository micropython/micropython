/* This linker script fragment is intended to be included in SECTIONS when
   common_ifs.ld is used with common_isr.ld
. */

.isr_extratext :
{
    . = ALIGN(4);

    /* This first flash block is 16K and the isr vectors only take up
        about 400 bytes. So we pull in a couple of object files to pad it
        out and save flash space in later blocks.

        NOTE: If you update the list of files contained here in .isr_extratext
        then be sure to also update stm32/Makefile where it builds each of
        these files with -Os to keep this section as compact as possible.
    */
    */ff.o(.text*)
    */vfs_fat_*.o(.text*)
    */py/formatfloat.o(.text*)
    */py/parsenum.o(.text*)
    */py/mpprint.o(.text*)

    . = ALIGN(4);
} >FLASH_ISR
