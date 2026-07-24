# Multi-volume FS: mount every SimpleFileSystem volume by hand and confirm at least one
# exposes the staged ESP contents through os/open (the `map`/`FSx:` capability — reaching
# a volume other than the boot mount at "/").
try:
    import os
    import uefi
    from uefi import fs as FS
except ImportError:
    print("SKIP")
    raise SystemExit

vols = FS.volumes()
print(len(vols) >= 1)
found = False
for i, h in enumerate(vols):
    pt = "/uefitest%d" % i
    try:
        FS.mount(h, pt)
    except OSError:
        continue
    try:
        entries = os.listdir(pt)
        if "samples" in entries or "EFI" in entries:
            found = True
            assert b"42" in open(pt + "/fsmod.py", "rb").read()  # read through the new mount
    except OSError:
        pass
    FS.umount(pt)
print(found)
print("fs multi-volume ok")
