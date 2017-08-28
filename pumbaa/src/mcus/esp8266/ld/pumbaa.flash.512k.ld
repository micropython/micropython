/**
 * Linker script for boards with 1 MB flash and 80k dram.
 */
MEMORY
{
  dport0_0_seg :                      	org = 0x3ff00000, len = 0x10
  dram0_0_seg :                       	org = 0x3ffe8000, len = 0x14000
  iram1_0_seg :                       	org = 0x40100000, len = 0x8000
  irom0_0_seg :                         org = 0x40201010, len = 0x7e000
}

INCLUDE "pumbaa.common.ld"
