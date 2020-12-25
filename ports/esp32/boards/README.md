ESP32 Board configurations
==========================

- `GENERIC` should work for most basic esp32 boards. It includes all functionality but ends up pretty
  tight in memory. Also consider adding sdkconfig.fast for better performance, unless your esp32
  chip is ancient or uses an odd xtal. Note that this build, as well as all others that include
  bluetooth, disables one core, i.e., everything runs on `core_0` and `core_1` is turned off.
- `GENERIC_D2WD` is for boards using the esp32-d2wd chip, which has 2MB flash inside and thus needs
  a different partition map. This chip is not very common.
- `GENERIC_OTA` is a variant of `GENERIC` with a partition table that supports two firmware
  partitions, thereby enabling Over-The-Air updates. The filesystem is correspondingly a bit
  smaller.
- `TINYPICO` is [https://www.tinypico.com](a tiny board) that contains an esp32-pico-d4, which has
  4MB flash on the chip. The board also supports SPIRAM and that is enabled.
- `TOOTHLESS_OTA` is for most esp32 boards but excludes Bluetooth functionality. This frees up
  some memory (especially for esp-idf and networking) and enables both cores. It also runs the
  flash chip at 80Mhz in QIO mode, which may be incompatible with some non-espressif boards.
