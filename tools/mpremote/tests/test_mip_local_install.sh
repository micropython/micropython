#!/bin/bash

# This test the "mpremote mip install" from local files. It creates a package
# and "mip installs" it into a ramdisk. The package is then imported and
# executed. The package is a simple "Hello, world!" example.

set -e

PACKAGE=mip_example
PACKAGE_DIR=${TMP}/example
MODULE_DIR=${PACKAGE_DIR}/${PACKAGE}

# Get the test directory (where this script and ramdisk.py are located)
TEST_DIR=$(dirname $0)

target=/__ramdisk

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

$MPREMOTE exec "mount_path='${target}'; do_chdir=False" run "${TEST_DIR}/ramdisk.py"
$MPREMOTE resume mkdir ${target}/lib
echo
echo ---- Install package
$MPREMOTE resume mip install --target=${target}/lib ${PACKAGE_DIR}/package.json
echo
echo ---- Test package
$MPREMOTE resume exec "import sys; sys.path.append(\"${target}/lib\")"
$MPREMOTE resume exec "import ${PACKAGE}; ${PACKAGE}.hello()"
