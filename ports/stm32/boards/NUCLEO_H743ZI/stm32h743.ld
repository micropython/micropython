/*
    GNU linker script for STM32H743

    The datasheet is not very clear on the exact lengths at page 109 so this needs extra
    checking. Also the _heap_end needs fixing, depending on the RAM map.
*/

/* Specify the memory areas */
MEMORY
{
  /* there are two banks, for now only the first is used, see RM0433, page 114 */

  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K   /* 8 sectors of 128Kib */
  FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 128K    /* sector  0, 128K */
  FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 640K    /* sectors 1..5 *128Kib */
  FLASH_FS (r)    : ORIGIN = 0x08008000, LENGTH = 256K    /* sectors 6..7 *128Kib */

  FLASH2 (rx)     : ORIGIN = 0x81000000, LENGTH = 1024K   /* 8 sectors of 128Kib */

  /* areas as indicated in RM0433, page 109, total of 864Kib, but not very informative */

  /* SRAM seems to be a lot 512Kib (R/W with ST-LINK up to 0x24080000), but not adjacent to SRAM1 lower boundary
   * SRAM1 and SRAM2 seems to be 128Kib each
   * SRAM3, SRAM4 unknown
   */
  SRAM  (xrw) : ORIGIN = 0x24000000, LENGTH = 512K    /* AXI D1 */
  SRAM1 (xrw) : ORIGIN = 0x30000000, LENGTH = 128K    /* AHB D2 */
  SRAM2 (xrw) : ORIGIN = 0x30020000, LENGTH = 128K
  SRAM3 (xrw) : ORIGIN = 0x30040000, LENGTH = xxxx
  SRAM4 (xrw) : ORIGIN = 0x38000000, LENGTH = xxxx    /* AHB D3 */

  /* set for the continuous space */
  RAM (xrw) : ORIGIN = AHB_SRAM1, LENGTH = (LENGTH(SRAM1)+LENGTH(SRAM2)+LENGTH(SRAM3))

  DTCM  (xrw) : ORIGIN = 0x20000000, LENGTH = 128K   /* disk cache */

  /* for completeness */
  ITCM  (xrw) : ORIGIN = 0x00000000, LENGTH = 64K    /* not used here */
  BSRAM (xrw) : ORIGIN = 0x38800000, LENGTH = 4K     /* not used here */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* define common sections and symbols */
INCLUDE common.ld

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
/*TODO: not correct, better to make it into a labeled address */
_heap_end = 0x20078000; /* tunable */
