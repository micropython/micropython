import uos
import builtins
from flashbdev import bdev

try:
    vfs = uos.VfsFat(bdev, "")
except OSError:
    import inisetup
    inisetup.check_bootsec()
    uos.VfsFat.mkfs(bdev)
    vfs = uos.VfsFat(bdev, "")
    inisetup.wifi()
