The following files are firmware that should work on most ESP32-S2-based
boards with 4MiB or more of flash, including WROOM, WROVER, and MINI modules.

This firmware supports configurations with and without SPIRAM (also known as
PSRAM) and will auto-detect a connected SPIRAM chip at startup and allocate
the MicroPython heap accordingly.
