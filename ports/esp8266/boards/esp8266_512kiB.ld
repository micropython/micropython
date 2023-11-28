/* GNU linker script for ESP8266 with 512K flash

   Flash layout:
    0x40200000 36k  header + iram/dram init
    0x40209000 456k firmware (irom0)
    0x4027b000 20k  SDK parameters
*/

MEMORY
{
    dport0_0_seg : org = 0x3ff00000, len = 16
    dram0_0_seg :  org = 0x3ffe8000, len = 80K
    iram1_0_seg :  org = 0x40100000, len = 32K
    irom0_0_seg :  org = 0x40209000, len = 512K - 36K - 20K
}

/* define common sections and symbols */
INCLUDE boards/esp8266_common.ld
