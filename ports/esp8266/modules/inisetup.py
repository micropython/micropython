import vfs
import network
from flashbdev import bdev


def read_all(fn):
    with open(fn, "rb") as fp:
        while fp.read(256):
            pass


def fwupdate(fn, erase_all=False, safe_check=True, verbose=True):
    import esp

    # get number of blocks
    fw_size = rem_fs = os.stat(fn)[6]
    esp.set_dfu(0, fw_size)
    read_all(fn)

    # allocate buffer for storing sector numbers and offsets
    nblks = esp.get_blks(False)
    blk_buf = esp.set_dfu(nblks, fw_size)
    if not blk_buf:
        raise MemoryError(f"Out of memory")

    # dig out block numbers and offsets using LFS2 hook
    esp.set_dfu(0, fw_size)
    read_all(fn)

    if safe_check:
        blks = esp.get_blks(True)
        if blks is None:
            raise Exception("No blocks data found")
        blks, offs = blks
        esp.set_dfu(-1, fw_size)
        if verbose:
            print(f"Verifying sector data:{blks}\n{offs}")
        with open(fn, "rb") as fp:
            for ii, blkid in enumerate(blks):
                sz = min(rem_fs, 4096 - offs[ii])
                L2 = esp.flash_read(esp.flash_user_start() + blkid * 4096 + offs[ii], sz)
                L1 = fp.read(sz)
                if L1 != L2:
                    raise Exception(f"Data is different at N={ii} blkid={blkid}")
                del L1, L2
                rem_fs -= sz
                if verbose:
                    print(f"{ii}/{len(blks)}", end="\r")
        esp.set_dfu(len(blks), fw_size)
        del blks, offs
        if verbose:
            print("Success, starting firmware update ...")

    esp.DFU(erase_all)


def wifi():
    ap_if = network.WLAN(network.AP_IF)
    ap_if.active(True)
    ssid = "MicroPython-" + ap_if.config("mac")[-3:].hex()
    ap_if.config(ssid=ssid, security=network.AUTH_WPA_WPA2_PSK, key=b"micropythoN")


def check_bootsec():
    buf = bytearray(bdev.SEC_SIZE)
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
    import micropython

    # Allow this loop to be stopped via Ctrl-C.
    micropython.kbd_intr(3)

    while 1:
        print(
            """\
The filesystem starting at sector %d with size %d sectors looks corrupt.
You may want to make a flash snapshot and try to recover it. Otherwise,
format it with vfs.VfsLfs2.mkfs(bdev), or completely erase the flash and
reprogram MicroPython.
"""
            % (bdev.start_sec, bdev.blocks)
        )
        time.sleep(3)


def setup():
    check_bootsec()
    print("Performing initial setup")
    wifi()
    vfs.VfsLfs2.mkfs(bdev)
    fs = vfs.VfsLfs2(bdev)
    vfs.mount(fs, "/")
    with open("boot.py", "w") as f:
        f.write(
            """\
# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import os, machine
#os.dupterm(None, 1) # disable REPL on UART(0)
import gc
#import webrepl
#webrepl.start()
gc.collect()
"""
        )
    return vfs
