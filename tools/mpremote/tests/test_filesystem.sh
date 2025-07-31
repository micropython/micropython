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
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume ls

echo -----
$MPREMOTE resume touch a.py
$MPREMOTE resume touch :b.py
$MPREMOTE resume ls :
$MPREMOTE resume cat a.py
$MPREMOTE resume cat :b.py
$MPREMOTE resume sha256sum a.py
echo -n "" | sha256sum

echo -----
cat << EOF > "${TMP}/a.py"
print("Hello")
print("World")
EOF
$MPREMOTE resume cp "${TMP}/a.py" :
$MPREMOTE resume cp "${TMP}/a.py" :b.py
$MPREMOTE resume cp "${TMP}/a.py" :c.py
$MPREMOTE resume cp :a.py :d.py
$MPREMOTE resume ls
$MPREMOTE resume exec "import a; import b; import c"
$MPREMOTE resume sha256sum a.py
cat "${TMP}/a.py" | sha256sum

echo -----
$MPREMOTE resume mkdir aaa
$MPREMOTE resume mkdir :bbb
$MPREMOTE resume cp "${TMP}/a.py" :aaa
$MPREMOTE resume cp "${TMP}/a.py" :bbb/b.py
$MPREMOTE resume cat :aaa/a.py bbb/b.py

# Test cp -f (force copy).
echo -----
$MPREMOTE resume cp -f "${TMP}/a.py" :aaa
$MPREMOTE resume cat :aaa/a.py

# Test cp where the destination has a trailing /.
echo -----
$MPREMOTE resume cp "${TMP}/a.py" :aaa/
$MPREMOTE resume cp "${TMP}/a.py" :aaa/a.py/ || echo "expect error"

echo -----
$MPREMOTE resume rm :b.py c.py
$MPREMOTE resume ls
$MPREMOTE resume rm :aaa/a.py bbb/b.py
$MPREMOTE resume rmdir aaa :bbb
$MPREMOTE resume ls

echo -----
env EDITOR="sed -i s/Hello/Goodbye/" $MPREMOTE resume edit d.py
$MPREMOTE resume sha256sum :d.py
$MPREMOTE resume exec "import d"


# Create a local directory structure and copy it to `:` on the device.
echo -----
mkdir -p "${TMP}/package"
mkdir -p "${TMP}/package/subpackage"
cat << EOF > "${TMP}/package/__init__.py"
from .x import x
from .subpackage import y
EOF
cat << EOF > "${TMP}/package/x.py"
def x():
  print("x")
EOF
cat << EOF > "${TMP}/package/subpackage/__init__.py"
from .y import y
EOF
cat << EOF > "${TMP}/package/subpackage/y.py"
def y():
  print("y")
EOF
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume cp -r "${TMP}/package" :
$MPREMOTE resume ls : :package :package/subpackage
$MPREMOTE resume exec "import package; package.x(); package.y()"


# Same thing except with a destination directory name.
echo -----
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume cp -r "${TMP}/package" :package2
$MPREMOTE resume ls : :package2 :package2/subpackage
$MPREMOTE resume exec "import package2; package2.x(); package2.y()"


# Copy to an existing directory, it will be copied inside.
echo -----
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume mkdir :test
$MPREMOTE resume cp -r "${TMP}/package" :test
$MPREMOTE resume ls :test :test/package :test/package/subpackage

# Copy to non-existing sub-directory.
echo -----
$MPREMOTE resume cp -r "${TMP}/package" :test/package2
$MPREMOTE resume ls :test :test/package2 :test/package2/subpackage

# Copy from the device back to local.
echo -----
mkdir "${TMP}/copy"
$MPREMOTE resume cp -r :test/package "${TMP}/copy"
ls "${TMP}/copy" "${TMP}/copy/package" "${TMP}/copy/package/subpackage"

# Copy from the device back to local with destination directory name.
echo -----
$MPREMOTE resume cp -r :test/package "${TMP}/copy/package2"
ls "${TMP}/copy" "${TMP}/copy/package2" "${TMP}/copy/package2/subpackage"


# Copy from device to another location on the device with destination directory name.
echo -----
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume cp -r "${TMP}/package" :
$MPREMOTE resume cp -r :package :package3
$MPREMOTE resume ls : :package3 :package3/subpackage

# Copy from device to another location on the device into an existing directory.
echo -----
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume cp -r "${TMP}/package" :
$MPREMOTE resume mkdir :package4
$MPREMOTE resume cp -r :package :package4
$MPREMOTE resume ls : :package4 :package4/package :package4/package/subpackage

# Repeat an existing copy with one file modified.
echo -----
cat << EOF > "${TMP}/package/subpackage/y.py"
def y():
  print("y2")
EOF
$MPREMOTE resume cp -r "${TMP}/package" :
$MPREMOTE resume ls : :package :package/subpackage
$MPREMOTE resume exec "import package; package.x(); package.y()"

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
# rm -r absolute root
# no -v to generate same output on stm32 and other ports
$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume touch :a.py
$MPREMOTE resume touch :b.py
$MPREMOTE resume cp -r "${TMP}/package" :
$MPREMOTE resume cp -r "${TMP}/package" :package2
$MPREMOTE resume rm -r :/ || echo "expect error"
$MPREMOTE resume ls :
$MPREMOTE resume ls :/ramdisk

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