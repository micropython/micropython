/* GNU linker script for ESP8266 with 2M or more flash

   Flash layout:
    0x40200000 36k  header + iram/dram init
    0x40209000 988k firmware (irom0)
    0x40300000 1M+  filesystem (not memory mapped)
*/

MEMORY
{
    dport0_0_seg : org = 0x3ff00000, len = 16
    dram0_0_seg :  org = 0x3ffe8000, len = 80K
    iram1_0_seg :  org = 0x40100000, len = 32K
    irom0_0_seg :  org = 0x40209000, len = 1M - 36K
}

/* define common sections and symbols */
INCLUDE boards/esp8266_common.ld
