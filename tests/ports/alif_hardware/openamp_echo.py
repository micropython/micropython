# MIT license; Copyright (c) 2026 OpenMV LLC.
#
# Test OpenAMP between the HP and HE cores.
#
# This test writes a small echo program to the HE core's ROMFS partition (the
# MRAM ROMFS partition, which is partition 1 on the HP core), starts the HE core
# with openamp.RemoteProc, then exchanges messages over an RPMsg endpoint.  The
# original contents of the HE ROMFS partition are restored at the end.
#
# Requirements:
# - HP firmware built with MICROPY_PY_OPENAMP=1 and MICROPY_PY_OPENAMP_REMOTEPROC=1.
# - HE firmware from the same build deployed at HE_ENTRY (0x80320000), eg using
#   `make BOARD=OPENMV_AE3 MCU_CORE=M55_DUAL deploy`.  Both cores must agree on
#   the location of the OpenAMP shared memory.

import time
import vfs

try:
    import openamp
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(vfs, "rom_ioctl") or not hasattr(openamp, "RemoteProc"):
    print("SKIP")
    raise SystemExit

HE_ENTRY = 0x80320000
HE_ROMFS_ID = 1
EPT_NAME = "mp-test"
MAX_PAYLOAD = 496  # RPMSG_BUFFER_SIZE (512) minus the 16 byte RPMsg header.
TIMEOUT_MS = 5000

# Program run by the HE core: echo everything received on the test endpoint.
HE_MAIN_PY = b"""\
import openamp, time
rx = []
def cb(src, data):
    rx.append(bytes(data))
ept = openamp.Endpoint("mp-test", callback=cb)
while True:
    while rx:
        ept.send(rx.pop(0), timeout=1000)
    time.sleep_ms(1)
"""


def romfs_encode_uint(value):
    encoded = [value & 0x7F]
    value >>= 7
    while value:
        encoded.insert(0, 0x80 | (value & 0x7F))
        value >>= 7
    return bytes(encoded)


def romfs_make_image(files):
    # Build a ROMFS image with the given files (see tools/mpremote/mpremote/romfs.py).
    data = bytearray()
    for name, content in files:
        name = bytes(name, "ascii")
        payload = romfs_encode_uint(len(name)) + name
        payload += romfs_encode_uint(2) + romfs_encode_uint(len(content)) + content
        data += romfs_encode_uint(5) + romfs_encode_uint(len(payload)) + payload
    header = b"\xd2\xcd\x31"
    encoded_len = romfs_encode_uint(len(data))
    if (len(header) + len(encoded_len) + len(data)) % 2:
        encoded_len = b"\x80" + encoded_len
    return header + encoded_len + data


def romfs_image_size(seg):
    # Return the size of the ROMFS image at the start of seg, or 0 if there is none.
    if bytes(seg[:3]) != b"\xd2\xcd\x31":
        return 0
    i = 3
    n = 0
    while True:
        b = seg[i]
        i += 1
        n = (n << 7) | (b & 0x7F)
        if not b & 0x80:
            break
    return i + n


def romfs_write(rom_id, image):
    min_write = vfs.rom_ioctl(3, rom_id, len(image))
    if min_write < 0:
        raise OSError(min_write)
    if len(image) % min_write:
        image = image + bytes(min_write - len(image) % min_write)
    chunk = 4096
    for offset in range(0, len(image), chunk):
        ret = vfs.rom_ioctl(4, rom_id, offset, image[offset : offset + chunk])
        if ret < 0:
            raise OSError(ret)
    vfs.rom_ioctl(5, rom_id)


def wait_for(cond, timeout_ms=TIMEOUT_MS):
    start = time.ticks_ms()
    while not cond():
        if time.ticks_diff(time.ticks_ms(), start) > timeout_ms:
            return False
        time.sleep_ms(1)
    return True


rx = []


def ept_callback(src, data):
    # The data is passed by reference, so it must be copied.
    rx.append(bytes(data))


# Save the current HE ROMFS image, and replace it with the echo program.
seg = vfs.rom_ioctl(2, HE_ROMFS_ID)
saved = bytes(seg[: romfs_image_size(seg)])
romfs_write(HE_ROMFS_ID, romfs_make_image([("main.py", HE_MAIN_PY)]))

rproc = None
try:
    # Register the endpoint, it will be bound when the HE core announces it.
    ept = openamp.Endpoint(EPT_NAME, callback=ept_callback)
    rproc = openamp.RemoteProc(HE_ENTRY)
    rproc.start()
    ready = wait_for(ept.is_ready)
    print("ready", ready)
    if not ready:
        raise SystemExit

    # Echo messages of various sizes.
    for size in (1, 16, 64, 255, 256, MAX_PAYLOAD):
        msg = bytes((i + size) & 0xFF for i in range(size))
        ept.send(msg, timeout=1000)
        ok = wait_for(lambda: rx)
        print("echo", size, ok and rx.pop(0) == msg)

    # Send a burst of messages and check they all come back, in order.
    count = 200
    for i in range(count):
        ept.send(i.to_bytes(4, "little") * 16, timeout=1000)
    ok = wait_for(lambda: len(rx) >= count)
    ordered = all(rx[i] == i.to_bytes(4, "little") * 16 for i in range(min(count, len(rx))))
    print("burst", ok, len(rx) == count, ordered)
finally:
    if rproc is not None:
        rproc.shutdown()
    if saved:
        romfs_write(HE_ROMFS_ID, saved)
    else:
        romfs_write(HE_ROMFS_ID, bytes(16))
