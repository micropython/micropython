The following files are firmware images that should work on most ESP32-C5-based
boards with at least 4MiB of flash and 40MHz/48MHz crystal frequency.

This firmware supports configurations with and without SPIRAM (also known as
PSRAM) and will auto-detect a connected SPIRAM chip at startup and allocate
the MicroPython heap accordingly.
