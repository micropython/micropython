from esp32 import Partition

# MicroPython's partition table uses "vfs", TinyUF2 uses "ffat".
bdev = Partition.find(Partition.TYPE_DATA, label="vfs")
if not bdev:
    bdev = Partition.find(Partition.TYPE_DATA, label="ffat", block_size=512)
bdev = bdev[0] if bdev else None

# If there are no explicit partitions, try to use the AUTOFS one.
if bdev is None:
    bdev = Partition(Partition.AUTOFS)
    if bdev.info()[3] == 0:
        # Empty partition, don't use it.
        bdev = None
