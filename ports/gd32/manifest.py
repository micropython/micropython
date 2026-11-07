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
#
# freeze("modules") registers each file by its bare name, so _boot.py becomes
# `_boot` (which is what main.c looks for) and gd32.py becomes `gd32` (which is
# what `import gd32` expects).  Listing the files with module() instead would
# register them as "modules/_boot" / "modules/gd32", and neither lookup would
# ever match.
freeze("modules")
