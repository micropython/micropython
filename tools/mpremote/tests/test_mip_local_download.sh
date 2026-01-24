#!/bin/bash

# This test the "mpremote mip download" from local files. It creates a package
# and downloads it into a local directory. The package contents are then
# verified on disk.

set -e

PACKAGE=mip_example
PACKAGE_DIR="${TMP}/example"
MODULE_DIR="${PACKAGE_DIR}/${PACKAGE}"

TARGET_DIR="${TMP}/lib"

echo ----- Setup
mkdir -p "${MODULE_DIR}"
echo "def hello(): print('Hello, world!')" > "${MODULE_DIR}/hello.py"
echo "from .hello import hello" > "${MODULE_DIR}/__init__.py"
cat > "${PACKAGE_DIR}/package.json" <<EOF
{
    "urls": [
        ["${PACKAGE}/__init__.py", "${PACKAGE}/__init__.py"],
        ["${PACKAGE}/hello.py", "${PACKAGE}/hello.py"]
    ],
    "version": "0.2"
}
EOF

echo
echo ---- Download package
"${MPREMOTE}" mip download --target="${TARGET_DIR}" "${PACKAGE_DIR}/package.json"
echo
echo ---- Verify package
diff -u "${MODULE_DIR}/__init__.py" "${TARGET_DIR}/${PACKAGE}/__init__.py"
diff -u "${MODULE_DIR}/hello.py" "${TARGET_DIR}/${PACKAGE}/hello.py"
