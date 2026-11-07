# manifest.py - list of modules frozen into the GD32F407/GD32F470 firmware.
#
# Freezing stores the compiled bytecode in Flash.  Compared with keeping the
# same code on the littlefs partition this:
#   - costs no RAM for source text,
#   - cannot be deleted or corrupted by the application,
#   - imports instantly because no parsing is needed.
#
# Use `make BOARD=... FROZEN_MANIFEST=...` to point at a different manifest.

# Default frozen modules for this port.
module("modules/_boot.py", opt=3)
module("modules/gd32.py", opt=3)
