The following files are firmware that should work on most ESP32-S3-based
boards with 4/8MiB of flash, including WROOM and MINI modules.

This firmware supports configurations with and without SPIRAM (also known as
PSRAM) and will auto-detect a connected SPIRAM chip at startup and allocate
the MicroPython heap accordingly. However if your board has Octal SPIRAM, then
use the "spiram-oct" variant.

If your board has 4MiB flash (including ESP32-S3FH4R2 based ones with embedded flash), then use the "flash-4m" build.
