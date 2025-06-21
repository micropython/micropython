The following files are firmware that should work on most ESP32-based boards
with 4MiB or more of flash, including WROOM WROVER, SOLO, PICO, and MINI modules.

This board has multiple variants available:

* If your board is based on a WROVER module, or otherwise has SPIRAM (also known
  as PSRAM) then it's recommended to use the "spiram" variant. Look for heading
  **Support for SPIRAM / WROVER)**.
* If your board has a ESP32-D2WD chip (with only 2MiB flash) then use the "d2wd"
  variant. Look for heading **ESP32 D2WD**.
* If your board has a single-core ESP32 (e.g. the "SOLO" modules) then choose
  the "unicore" variant. Look for heading **ESP32 Unicore**.
* If you'd like to perform Over-the-Air updates of the MicroPython firmware,
  then choose the "ota" variant. This variant has less room in the flash for
  Python files as a result of supporting OTA. Look for heading **Support for
  OTA**.

Otherwise, download the generic variant (under the first heading below).
