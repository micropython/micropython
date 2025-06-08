#!/bin/bash
# Test script to build and test dual-mode module

set -e

echo "=== Testing Dual-Mode Module ==="
echo

# Test 1: Build as native module
echo "1. Building as native module (.mpy)..."
cd /home/corona/micropython/examples/dualmode
rm -rf build dualmode_simple.mpy
make -f Makefile.simple ARCH=x64
ls -la dualmode_simple.mpy

echo
echo "2. Testing native module..."
cd /home/corona/micropython/ports/unix
./build-standard/micropython -c "
import sys
sys.path.insert(0, '/home/corona/micropython/examples/dualmode')
import dualmode_simple
print('Native module loaded successfully')
print('add(10, 20) =', dualmode_simple.add(10, 20))
print('hello() =', dualmode_simple.hello())
"

echo
echo "3. Building Unix port with dualmode as user C module..."
cd /home/corona/micropython/ports/unix
# First clean build
make clean
# Build with verbose to see what's happening
make V=1 USER_C_MODULES=/home/corona/micropython/examples/dualmode 2>&1 | grep -E "(dualmode|USERMOD)" | head -20

echo
echo "4. Checking if module was built in..."
if ./build-standard/micropython -c "import dualmode_simple; print('User C module loaded successfully')" 2>/dev/null; then
    echo "SUCCESS: Module works as user C module"
    ./build-standard/micropython -c "
import dualmode_simple
print('add(15, 25) =', dualmode_simple.add(15, 25))
print('hello() =', dualmode_simple.hello())
"
else
    echo "FAILED: Module not available as user C module"
    echo "Checking what modules are available:"
    ./build-standard/micropython -c "help('modules')" | grep -v "Plus any" | sort
fi