import sys, os, alif


bdev = alif.Flash()
try:
    vfs = os.VfsLfs2(bdev, progsize=256)
except:
    os.VfsLfs2.mkfs(bdev, progsize=256)
    vfs = os.VfsLfs2(bdev, progsize=256)
os.mount(vfs, "/flash")

sys.path.append("/flash")
sys.path.append("/flash/lib")
os.chdir("/flash")

del sys, os, alif, bdev, vfs
