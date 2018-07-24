from flashbdev import bdev
import network
import storage

def wifi():
    try:
        import ubinascii as binascii
    except ImportError:
        import binascii

    ap_if = network.WLAN(network.AP_IF)
    essid = b"MicroPython-%s" % binascii.hexlify(ap_if.config("mac")[-3:])
    ap_if.config(essid=essid, authmode=network.AUTH_WPA_WPA2_PSK, password=b"micropythoN")

def check_bootsec():
    buf = bytearray(bdev.SEC_SIZE)
    bdev.readblocks(0, buf)
    empty = True
    for b in buf:
        if b != 0xff:
            empty = False
            break
    if empty:
        return True
    fs_corrupted()

def fs_corrupted():
    import time
    while 1:
        print("""\
The FAT filesystem starting at sector %d with size %d sectors appears to
be corrupted. If you had important data there, you may want to make a flash
snapshot to try to recover it. Otherwise, perform factory reprogramming
of MicroPython firmware (completely erase flash, followed by firmware
programming).
""" % (bdev.START_SEC, bdev.blocks))
        time.sleep(3)

def setup():
    check_bootsec()
    print("Performing initial setup")
    wifi()
    storage.VfsFat.mkfs(bdev)
    vfs = storage.VfsFat(bdev)
    storage.mount(vfs, '/')
    with open("boot.py", "w") as f:
        f.write("""\
# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import gc
#import webrepl
#webrepl.start()
gc.collect()
""")
    return vfs
