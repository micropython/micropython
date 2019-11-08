/* GNU linker script for ESP8266 */

MEMORY
{
    dport0_0_seg : org = 0x3ff00000, len = 0x10
    dram0_0_seg :  org = 0x3ffe8000, len = 0x14000
    iram1_0_seg :  org = 0x40100000, len = 0x8000
    /* 0x3c000 is size of bootloader, 0x9000 is size of packed RAM segments */
    irom0_0_seg :  org = 0x40200000 + 0x3c000 + 0x9000, len = 0x8f000
}

/* define common sections and symbols */
INCLUDE boards/esp8266_common.ld
