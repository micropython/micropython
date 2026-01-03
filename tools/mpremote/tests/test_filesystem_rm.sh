#!/bin/bash
set -e

# Creates a RAM disk big enough to hold two copies of the test directory
# structure.
cat << EOF > "${TMP}/ramdisk.py"
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
        if op == 4: # get number of blocks
            return len(self.data) // self.block_size
        if op == 5: # get block size
            return self.block_size

import os

bdev = RAMBlockDev(512, 50)
os.VfsFat.mkfs(bdev)
os.mount(bdev, '/ramdisk')
os.chdir('/ramdisk')
EOF


echo -----
# Test rm -r functionality
# start with a fresh ramdisk before each test
# rm -r MCU current working directory
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume touch :a.py
$MPREMOTE resume touch :b.py
$MPREMOTE resume cp -r "${TMP}/package" :
$MPREMOTE resume rm -r -v :
$MPREMOTE resume ls :
$MPREMOTE resume ls :/ramdisk

echo -----
# rm -r relative subfolder
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume touch :a.py
$MPREMOTE resume mkdir :testdir
$MPREMOTE resume cp -r "${TMP}/package" :testdir/package
$MPREMOTE resume ls :testdir
$MPREMOTE resume ls :testdir/package
$MPREMOTE resume rm -r :testdir/package
$MPREMOTE resume ls :/ramdisk
$MPREMOTE resume ls :testdir

echo -----
# rm -r non-existent path
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume ls :
$MPREMOTE resume rm -r :nonexistent || echo "expect error"

echo -----
# rm -r relative mountpoint
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume touch :a.py
$MPREMOTE resume touch :b.py
$MPREMOTE resume cp -r "${TMP}/package" :
$MPREMOTE resume exec "import os;os.chdir('/')"
$MPREMOTE resume rm -r -v :ramdisk
$MPREMOTE resume ls :/ramdisk

echo -----
# rm -r absolute mountpoint
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume touch :a.py
$MPREMOTE resume touch :b.py
$MPREMOTE resume cp -r "${TMP}/package" :
$MPREMOTE resume exec "import os;os.chdir('/')"
$MPREMOTE resume rm -r -v :/ramdisk
$MPREMOTE resume ls :/ramdisk

echo -----
# try to delete existing folder in mounted filesystem
$MPREMOTE mount "${TMP}" + rm -rv :package || echo "expect error"
echo -----
# fs without command should raise error
$MPREMOTE fs 2>/dev/null || echo "expect error: $?"
echo -----
