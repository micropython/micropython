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

The seven buttons are active-low GPIOs, see `pins.csv`. `board.buttons` wraps
them so `board.buttons.Up().value()` reads 1 while pressed, and
`board.buttons.Up().irq(handler)` fires on press. Up is GPIO0, so holding it
during reset enters the ROM bootloader.

`board.get_battery_voltage()` and `board.get_battery_percentage()` read the
CW2017 gauge on I2C 0x63. If the gauge has lost its battery profile (the
battery was disconnected), the first percentage read uploads the stock X4C
profile, which takes up to 3 seconds. `board.is_charging()` reads the charger
status on GPIO21.

The microSD card uses SDMMC 1-bit on GPIO40-42 with an active-low power rail
on GPIO6. `board.sdcard()` powers the card and returns a `machine.SDCard`,
e.g. `vfs.mount(board.sdcard(), "/sd")`. `board.sd_power(False)` turns the
rail off again after `SDCard.deinit()`.
