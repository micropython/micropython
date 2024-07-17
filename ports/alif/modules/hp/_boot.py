import sys, os, alif


bdev = alif.Flash()
if hasattr(alif, "usb_msc"):
    try:
        # This may fail on VfsFat construction, or mount.
        os.mount(os.VfsFat(bdev), "/flash")
    except:
        os.VfsFat.mkfs(bdev)
        os.mount(os.VfsFat(bdev), "/flash")
else:
    try:
        os.mount(os.VfsLfs2(bdev, progsize=256), "/flash")
    except:
        os.VfsLfs2.mkfs(bdev, progsize=256)
        os.mount(os.VfsLfs2(bdev, progsize=256), "/flash")

sys.path.append("/flash")
sys.path.append("/flash/lib")
os.chdir("/flash")

del sys, os, alif, bdev
