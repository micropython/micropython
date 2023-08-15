The following files are firmware that should work on most ESP32-based boards
with 4MiB of flash, including WROOM WROVER, SOLO, PICO, and MINI modules.

If your board is based on a WROVER module, or otherwise has SPIRAM (also known
as PSRAM), then use the "spiram" variant.

The "d2wd" variant is for ESP32-D2WD chips (with 2MiB flash), and "unicore" is
for single-core ESP32 chips (e.g. the "SOLO" modules). The "ota" variant sets
up the partition table to allow for Over-the-Air updates.
