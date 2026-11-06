# _boot.py - runs automatically at startup when frozen into the firmware.
#
# Anything here executes before the REPL comes up, so keep it short: it lives
# in Flash and does not consume the Python heap.
import gc
import os

gc.collect()

# Mount the internal littlefs filesystem if it is not already mounted, so that
# scripts written to the device survive a reset.
try:
    os.stat("/")
except OSError:
    try:
        import vfs

        vfs.mount(vfs.VfsLfs2(bdev=__import__("machine").SDCard), "/")
    except Exception:
        pass
