The M5Stack CoreMatrix has an ESP32-C61HR8 with 16 MiB of flash and
8 MiB of PSRAM. The USB-C port provides the MicroPython REPL and normally
also supports automatic entry into the bootloader. Long-press the power
button to force download mode if automatic entry fails.

The onboard peripherals and Grove port share I2C bus 0 on GPIO0 (SDA)
and GPIO1 (SCL). The SD slot uses SPI and, like the LED matrix, requires
its power rail to be enabled via the onboard M5IOE1 I/O expander.
Peripheral drivers are not included in the firmware.