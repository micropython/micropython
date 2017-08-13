import uos
from flashbdev import bdev

try:
    if bdev:
        vfs = uos.VfsFat(bdev, "")
except OSError:
    import inisetup
    vfs = inisetup.setup()
