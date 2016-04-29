import uos
import builtins
from flashbdev import bdev

try:
    if bdev:
        vfs = uos.VfsFat(bdev, "")
except OSError:
    import inisetup
    vfs = inisetup.setup()
