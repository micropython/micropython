# _boot.py - runs at startup because it is frozen into the firmware.
#
# Keep this short: it executes before the REPL appears.  The internal littlefs
# filesystem is already mounted by main.c (mount_internal_fs) before this runs,
# so there is nothing to mount here.
import gc

gc.collect()

# If the filesystem mount failed because the partition is unformatted, the
# root is simply empty.  Formatting from here would overwrite user data, so we
# only report it and leave recovery to the user.
try:
    import os

    if not os.listdir("/"):
        print("littlefs: root is empty (run machine.Flash.mkfs() to format)")
except OSError:
    print("littlefs: root not mounted")
