The Xteink X4 Classic is a pocket eInk eReader built around the ESP32-S3, with
16MiB of flash, 8MiB of octal PSRAM, a 4.26" 800x480 monochrome eInk display
(UC8279 controller on the units tested), a microSD card slot, seven buttons,
a CW2017 battery gauge, a BM8563 RTC and a QMI8658 IMU.

It is a different board from the original ESP32-C3 based X4 and needs its own
firmware.

GPIO1 latches the main power rail. The firmware drives it high early in boot.
Don't reconfigure `Pin.board.POWER_LATCH` as an input or drive it low unless
you intend to switch the board off.

The REPL is on the native USB-Serial/JTAG interface.

The partition table matches the two 6.25MiB OTA slots used by CrossPoint,
so MicroPython runs from `ota_0` and `ota_1` is free for another firmware.
Back up the stock firmware with `esptool.py read_flash 0 0x1000000 backup.bin`
before erasing the flash.
