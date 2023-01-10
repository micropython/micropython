import uos
from flashbdev import bdev

str_corrupted = """
The filesystem appears to be corrupted. If you had important data there, you
may want to make a flash snapshot to try to recover it. Otherwise, perform
factory reprogramming of MicroPython firmware (completely erase flash, followed
by firmware programming).
"""
str_boot_py = """
# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
#import webrepl
#webrepl.start()
"""


def check_bootsec():
    buf = bytearray(bdev.ioctl(5, 0))  # 5 is SEC_SIZE
    bdev.readblocks(0, buf)
    empty = True
    for b in buf:
        if b != 0xFF:
            empty = False
            break
    if empty:
        return True
    fs_corrupted()


def fs_corrupted():
    import time

    while 1:
        print(str_corrupted)
        time.sleep(3)


def setup():
    check_bootsec()
    print("Performing initial setup")
    try:
        uos.VfsLfs2.mkfs(bdev)
        vfs = uos.VfsLfs2(bdev)
    except OSError:
        uos.VfsFat.mkfs(bdev)
        vfs = uos.VfsFat(bdev)
    uos.mount(vfs, "/")
    with open("boot.py", "w") as f:
        f.write(str_boot_py)
    return vfs
