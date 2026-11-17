# Creates a RAM disk for testing.
# Usage: mpremote run ramdisk.py
# Or with custom parameters:
#   mpremote exec "block_size=1024; num_blocks=100; mount_path='/myram'" run ramdisk.py
#   mpremote exec "mount_path='/__ramdisk'; do_chdir=False" run ramdisk.py
# Defaults: block_size=512, num_blocks=50, mount_path='/ramdisk', do_chdir=True

import os


class RAMBlockDev:
    def __init__(self, block_size, num_blocks):
        self.block_size = block_size
        self.data = bytearray(block_size * num_blocks)

    def readblocks(self, block_num, buf):
        for i in range(len(buf)):
            buf[i] = self.data[block_num * self.block_size + i]

    def writeblocks(self, block_num, buf):
        for i in range(len(buf)):
            self.data[block_num * self.block_size + i] = buf[i]

    def ioctl(self, op, arg):
        if op == 4:  # get number of blocks
            return len(self.data) // self.block_size
        if op == 5:  # get block size
            return self.block_size


# Use globals if set via exec, otherwise use defaults
try:
    block_size
except NameError:
    block_size = 512

try:
    num_blocks
except NameError:
    num_blocks = 50

try:
    mount_path
except NameError:
    mount_path = "/ramdisk"

try:
    do_chdir
except NameError:
    do_chdir = True

bdev = RAMBlockDev(block_size, num_blocks)
os.VfsFat.mkfs(bdev)
os.mount(bdev, mount_path)
if do_chdir:
    os.chdir(mount_path)
