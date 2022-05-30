# Connaxio's Espoir
Espoir is a Power over Ethernet Plus (PoE+ 802.3af/at) mikroBUS mainboard based on the ESP32.

This board requires the firmware to be compiled with ESP-IDF 4.4 or later to function properly, as it depends on the KSZ8081/KSZ8091 Ethernet PHY.

Some ESP32-MINI-1 are single core, as the underlying ESP32-U4WDH was changed from single core to double core in January 2022. This firmware is therefore compiled with the directive `CONFIG_FREERTOS_UNICORE=y` to support all chip versions.
