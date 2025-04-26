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

# setup 
echo -----
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume ls

echo -----
echo "empty tree"
$MPREMOTE resume tree :

echo -----
$MPREMOTE resume touch :a.py + touch :b.py  
$MPREMOTE resume mkdir :foo + touch :foo/aa.py + touch :foo/ba.py

echo "small tree - :" 
$MPREMOTE resume tree :

echo -----
echo "no path" 
$MPREMOTE resume tree 

echo -----
echo "path = '.'" 
$MPREMOTE resume tree .

echo -----
echo "path = ':.'" 
$MPREMOTE resume tree :.


echo -----
echo "multiple trees" 
$MPREMOTE resume mkdir :bar + touch :bar/aaa.py + touch :bar/bbbb.py
$MPREMOTE resume mkdir :bar/baz + touch :bar/baz/aaa.py + touch :bar/baz/bbbb.py
$MPREMOTE resume mkdir :bar/baz/quux + touch :bar/baz/quux/aaa.py + touch :bar/baz/quux/bbbb.py
$MPREMOTE resume mkdir :bar/baz/quux/xen + touch :bar/baz/quux/xen/aaa.py

$MPREMOTE resume tree

echo -----
echo single path
$MPREMOTE resume tree :foo

echo -----
echo "multiple paths" 
$MPREMOTE resume tree :foo :bar

echo -----
echo "subtree" 
$MPREMOTE resume tree bar/baz

echo -----
echo mountpoint
$MPREMOTE resume tree :/ramdisk

echo -----
echo non-existent folder : error
$MPREMOTE resume tree :not_there || echo "expect error: $?"

echo -----
echo file : error 
$MPREMOTE resume tree :a.py || echo "expect error: $?"

echo -----
echo "tree -s :"
mkdir -p "${TMP}/data"
dd if=/dev/zero of="${TMP}/data/file1.txt" bs=1 count=20 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file2.txt" bs=1 count=204 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file3.txt" bs=1 count=1096 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file4.txt" bs=1 count=2192 > /dev/null 2>&1

$MPREMOTE resume cp -r "${TMP}/data" :
$MPREMOTE resume tree -s :
echo -----
echo "tree -s"
$MPREMOTE resume tree -s
echo -----
$MPREMOTE resume tree --human :
echo -----
$MPREMOTE resume tree -s --human : || echo "expect error: $?"
echo -----

