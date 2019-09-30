/* GNU linker script for ESP8266 */

MEMORY
{
    dport0_0_seg : org = 0x3ff00000, len = 0x10
    dram0_0_seg :  org = 0x3ffe8000, len = 0x14000
    iram1_0_seg :  org = 0x40100000, len = 0x8000
    irom0_0_seg :  org = 0x40209000, len = 0x8f000
}

/* define common sections and symbols */
INCLUDE esp8266_common.ld
