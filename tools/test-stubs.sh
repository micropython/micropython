#!/bin/sh -e
rm -rf test-stubs
python3 -m venv test-stubs
. test-stubs/bin/activate
pip install mypy isort black adafruit-circuitpython-typing wheel build
rm -rf circuitpython-stubs .mypy_cache
make stubs
# Allow either dash or underscore as separator. setuptools v69.3.0 changed from "-" to "_".
pip install --force-reinstall circuitpython-stubs/dist/circuitpython[-_]stubs-*.tar.gz
export MYPYPATH=circuitpython-stubs/
echo "The following test should pass:"
mypy -c 'import busio; b: busio.I2C; b.writeto(0x30, b"")'
echo "The next two tests are expected to show type errors:"
! mypy -c 'import busio; b: busio.I2C; b.readfrom_into(0x30, b"")'
! mypy -c 'import busio; b: busio.I2C; b.write(0x30, b"")'
echo "Typing tests complete"
