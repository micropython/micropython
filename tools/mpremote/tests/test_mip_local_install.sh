#!/bin/bash

# This test the "mpremote mip install" from local files. It creates a package
# and "mip installs" it into a ramdisk. The package is then imported and
# executed. The package is a simple "Hello, world!" example.

set -e

PACKAGE=mip_example
PACKAGE_DIR=${TMP}/example
MODULE_DIR=${PACKAGE_DIR}/${PACKAGE}

target=/__ramdisk
block_size=512
num_blocks=50

# Create the smallest permissible ramdisk.
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

bdev = RAMBlockDev(${block_size}, ${num_blocks})
os.VfsFat.mkfs(bdev)
os.mount(bdev, '${target}')
EOF

echo ----- Setup
mkdir -p ${MODULE_DIR}
echo "def hello(): print('Hello, world!')" > ${MODULE_DIR}/hello.py
echo "from .hello import hello" > ${MODULE_DIR}/__init__.py
cat > ${PACKAGE_DIR}/package.json <<EOF
{
    "urls": [
        ["${PACKAGE}/__init__.py", "${PACKAGE}/__init__.py"],
        ["${PACKAGE}/hello.py", "${PACKAGE}/hello.py"]
    ],
    "version": "0.2"
}
EOF

$MPREMOTE run "${TMP}/ramdisk.py"
$MPREMOTE resume mkdir ${target}/lib
echo
echo ---- Install package
$MPREMOTE resume mip install --target=${target}/lib ${PACKAGE_DIR}/package.json
echo
echo ---- Test package
$MPREMOTE resume exec "import sys; sys.path.append(\"${target}/lib\")"
$MPREMOTE resume exec "import ${PACKAGE}; ${PACKAGE}.hello()"
