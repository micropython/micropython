#!/bin/bash
set -e

# Get the test directory (where this script and ramdisk.py are located)
TEST_DIR=$(dirname $0)

$MPREMOTE exec "import os; os.VfsFat" || { echo "SKIP (ramdisk not supported)"; exit 0; }

echo -----
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE ls

echo -----
$MPREMOTE touch a.py
$MPREMOTE touch :b.py
$MPREMOTE ls :
$MPREMOTE cat a.py
$MPREMOTE cat :b.py
$MPREMOTE sha256sum a.py
echo -n "" | sha256sum

echo -----
cat << EOF > "${TMP}/a.py"
print("Hello")
print("World")
EOF
$MPREMOTE cp "${TMP}/a.py" :
$MPREMOTE cp "${TMP}/a.py" :b.py
$MPREMOTE cp "${TMP}/a.py" :c.py
$MPREMOTE cp :a.py :d.py
$MPREMOTE ls
$MPREMOTE exec "import a; import b; import c"
$MPREMOTE sha256sum a.py
cat "${TMP}/a.py" | sha256sum

echo -----
$MPREMOTE mkdir aaa
$MPREMOTE mkdir :bbb
$MPREMOTE cp "${TMP}/a.py" :aaa
$MPREMOTE cp "${TMP}/a.py" :bbb/b.py
$MPREMOTE cat :aaa/a.py bbb/b.py

# Test cp -f (force copy).
echo -----
$MPREMOTE cp -f "${TMP}/a.py" :aaa
$MPREMOTE cat :aaa/a.py

# Test cp where the destination has a trailing /.
echo -----
$MPREMOTE cp "${TMP}/a.py" :aaa/
$MPREMOTE cp "${TMP}/a.py" :aaa/a.py/ || echo "expect error"

echo -----
$MPREMOTE rm :b.py c.py
$MPREMOTE ls
$MPREMOTE rm :aaa/a.py bbb/b.py
$MPREMOTE rmdir aaa :bbb
$MPREMOTE ls

echo -----
env EDITOR="sed -i s/Hello/Goodbye/" $MPREMOTE edit d.py
$MPREMOTE sha256sum :d.py
$MPREMOTE exec "import d"


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
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE cp -r "${TMP}/package" :
$MPREMOTE ls : :package :package/subpackage
$MPREMOTE exec "import package; package.x(); package.y()"


# Same thing except with a destination directory name.
echo -----
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE cp -r "${TMP}/package" :package2
$MPREMOTE ls : :package2 :package2/subpackage
$MPREMOTE exec "import package2; package2.x(); package2.y()"


# Copy to an existing directory, it will be copied inside.
echo -----
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE mkdir :test
$MPREMOTE cp -r "${TMP}/package" :test
$MPREMOTE ls :test :test/package :test/package/subpackage

# Copy to non-existing sub-directory.
echo -----
$MPREMOTE cp -r "${TMP}/package" :test/package2
$MPREMOTE ls :test :test/package2 :test/package2/subpackage

# Copy from the device back to local.
echo -----
mkdir "${TMP}/copy"
$MPREMOTE cp -r :test/package "${TMP}/copy"
ls "${TMP}/copy" "${TMP}/copy/package" "${TMP}/copy/package/subpackage"

# Copy from the device back to local with destination directory name.
echo -----
$MPREMOTE cp -r :test/package "${TMP}/copy/package2"
ls "${TMP}/copy" "${TMP}/copy/package2" "${TMP}/copy/package2/subpackage"


# Copy from device to another location on the device with destination directory name.
echo -----
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE cp -r "${TMP}/package" :
$MPREMOTE cp -r :package :package3
$MPREMOTE ls : :package3 :package3/subpackage

# Copy from device to another location on the device into an existing directory.
echo -----
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE cp -r "${TMP}/package" :
$MPREMOTE mkdir :package4
$MPREMOTE cp -r :package :package4
$MPREMOTE ls : :package4 :package4/package :package4/package/subpackage

# Repeat an existing copy with one file modified.
echo -----
cat << EOF > "${TMP}/package/subpackage/y.py"
def y():
  print("y2")
EOF
$MPREMOTE cp -r "${TMP}/package" :
$MPREMOTE ls : :package :package/subpackage
$MPREMOTE exec "import package; package.x(); package.y()"

echo -----
# Test rm -r functionality
# start with a fresh ramdisk before each test
# rm -r MCU current working directory
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE touch :a.py
$MPREMOTE touch :b.py
$MPREMOTE cp -r "${TMP}/package" :
$MPREMOTE rm -r -v :
$MPREMOTE ls :
$MPREMOTE ls :/ramdisk

echo -----
# rm -r relative subfolder
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE touch :a.py
$MPREMOTE mkdir :testdir
$MPREMOTE cp -r "${TMP}/package" :testdir/package
$MPREMOTE ls :testdir
$MPREMOTE ls :testdir/package
$MPREMOTE rm -r :testdir/package
$MPREMOTE ls :/ramdisk
$MPREMOTE ls :testdir

echo -----
# rm -r non-existent path
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE ls :
$MPREMOTE rm -r :nonexistent || echo "expect error"

echo -----
# rm -r absolute root
# no -v to generate same output on stm32 and other ports
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE touch :a.py
$MPREMOTE touch :b.py
$MPREMOTE cp -r "${TMP}/package" :
$MPREMOTE cp -r "${TMP}/package" :package2
$MPREMOTE rm -r :/ || echo "expect error"
$MPREMOTE ls :
$MPREMOTE ls :/ramdisk

echo -----
# rm -r relative mountpoint
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE touch :a.py
$MPREMOTE touch :b.py
$MPREMOTE cp -r "${TMP}/package" :
$MPREMOTE exec "import os;os.chdir('/')"
$MPREMOTE rm -r -v :ramdisk
$MPREMOTE ls :/ramdisk

echo -----
# rm -r absolute mountpoint
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE touch :a.py
$MPREMOTE touch :b.py
$MPREMOTE cp -r "${TMP}/package" :
$MPREMOTE exec "import os;os.chdir('/')"
$MPREMOTE rm -r -v :/ramdisk
$MPREMOTE ls :/ramdisk

echo -----
# try to delete existing folder in mounted filesystem
$MPREMOTE mount "${TMP}" + rm -rv :package || echo "expect error"
echo -----
# fs without command should raise error
$MPREMOTE fs 2>/dev/null || echo "expect error: $?"
echo -----
