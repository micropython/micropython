#!/bin/bash
set -e

# Create a local directory structure and mount the parent directory on the device.
echo -----
mkdir -p "${TMP}/mount_package"
mkdir -p "${TMP}/mount_package/subpackage"
cat << EOF > "${TMP}/mount_package/__init__.py"
from .x import x
from .subpackage import y
EOF
cat << EOF > "${TMP}/mount_package/x.py"
def x():
  print("x")
EOF
cat << EOF > "${TMP}/mount_package/subpackage/__init__.py"
from .y import y
EOF
cat << EOF > "${TMP}/mount_package/subpackage/y.py"
def y():
  print("y")
EOF
$MPREMOTE mount ${TMP} exec "import mount_package; mount_package.x(); mount_package.y()"

# Write to a file on the device and see that it's written locally.
echo -----
$MPREMOTE mount ${TMP} exec "open('test.txt', 'w').write('hello world\n')"
cat "${TMP}/test.txt"

# Test RemoteFile.readline and RemoteFile.readlines methods.
echo -----
$MPREMOTE mount ${TMP} exec "print(open('test.txt').readlines())"
