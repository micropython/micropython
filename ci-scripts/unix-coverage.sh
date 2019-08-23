#!/usr/bin/env sh

set -x

git submodule update --init lib/axtls lib/berkeley-db-1.xx lib/libffi
make ${MAKEOPTS} -C mpy-cross
make ${MAKEOPTS} -C ports/unix deplibs
make ${MAKEOPTS} -C ports/unix coverage

# run the main test suite
(
cd tests
export MICROPY_CPYTHON3=python3
export MICROPY_MICROPYTHON=../ports/unix/micropython_coverage
./run-tests
./run-tests -d thread
./run-tests --emit native
./run-tests --via-mpy -d basics float micropython
./run-tests --via-mpy --emit native -d basics float micropython
)

# test when input script comes from stdin
cat tests/basics/0prelim.py | ports/unix/micropython_coverage | grep -q 'abc'

# run coveralls coverage analysis (try to, even if some builds/tests failed)
(
cd ports/unix
coveralls --root ../.. --build-root . --gcov $(which gcov) --gcov-options '\-o build-coverage/' --include py --include extmod
)
