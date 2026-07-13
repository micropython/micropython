# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# boot_disk.py — boot a kernel + initramfs from the boot volume.
#   exec(open('/samples/boot_disk.py').read())
#
# Reads a kernel and an initramfs off the mounted boot volume into memory and boots
# via the EFI mechanism: LoadImage the kernel, present the initramfs from RAM with
# EFI_LOAD_FILE2_PROTOCOL, StartImage. This is the local-disk counterpart to
# net_boot.py; it isolates the LoadImage / LoadFile2 / StartImage path (no network).
#
# The default paths are the minimal aarch64 test OS from
# ports/uefi/test-images/build-test-os.sh (stage them onto the boot volume first).

import os
import uctypes
import uefi
from uefi import load_file
from uefi.buffer import PoolBuffer, LOADER_DATA

KERNEL = "/vmlinuz-aa64"
INITRD = "/initramfs-aa64.cpio.gz"
# Serial on both the aa64 'virt' (ttyAMA0) and x64 q35 (ttyS0) QEMU machines.
CMDLINE = "console=ttyAMA0 console=ttyS0"


def read_to_pool(path):
    """Read `path` off the boot volume straight into a pool buffer (no big bytes)."""
    size = os.stat(path)[6]
    buf = PoolBuffer(size, memory_type=LOADER_DATA)
    got = 0
    with open(path, "rb") as f:
        while got < size:
            view = uctypes.bytearray_at(buf.ptr + got, min(65536, size - got))
            n = f.readinto(view)
            if not n:
                break
            got += n
    if got != size:
        buf.close()
        raise OSError("short read %d/%d for %s" % (got, size, path))
    return buf


print("Reading kernel:  %s" % KERNEL)
kernel = read_to_pool(KERNEL)
print("  %d bytes" % kernel.size)
print("Reading initrd:  %s" % INITRD)
initrd = read_to_pool(INITRD)
print("  %d bytes" % initrd.size)

# Present the initramfs from RAM; the kernel's EFI stub pulls it via LoadFile2.
provider = load_file.present_initrd(initrd)  # keep alive across start()

st, img_handle = uefi.raw.load_image(False, uefi.raw.image_handle(), 0, kernel.ptr, kernel.size)
uefi.status.check(st)
image = uefi.Image(img_handle)
image.set_load_options(CMDLINE)

print("Booting Linux (%s)..." % CMDLINE)
exit_status, _ = image.start()
print("kernel exited: 0x%x" % exit_status)  # only if it did not boot
