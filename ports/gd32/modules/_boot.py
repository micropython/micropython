# _boot.py - runs at startup because it is frozen into the firmware.
#
# Keep this short: it executes before the REPL appears.  The internal littlefs
# filesystem is already mounted by main.c (mount_internal_fs) before this runs.

import gc

gc.collect()

# Distinguish "nothing is mounted" from "mounted but empty".  os.listdir("/")
# returns [] in both cases, so probe the mount point itself: stat() only
# succeeds when a filesystem is actually mounted at "/".
try:
    import os

    os.stat("/")
except OSError:
    print("littlefs: not mounted (os.umount('/'); machine.Flash().mkfs())")
