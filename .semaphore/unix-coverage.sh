#!/usr/bin/env bash

set -x

git submodule update --init lib/axtls lib/berkeley-db-1.xx lib/libffi
make ${MAKEOPTS} -C mpy-cross
make ${MAKEOPTS} -C ports/unix deplibs
make ${MAKEOPTS} -C ports/unix coverage
# run the main test suite
(cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/micropython_coverage ./run-tests)
(cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/micropython_coverage ./run-tests -d thread)
(cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/micropython_coverage ./run-tests --emit native)
(cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/micropython_coverage ./run-tests --via-mpy -d basics float micropython)
(cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/micropython_coverage ./run-tests --via-mpy --emit native -d basics float micropython)
# test when input script comes from stdin
cat tests/basics/0prelim.py | ports/unix/micropython_coverage | grep -q 'abc'
# run coveralls coverage analysis (try to, even if some builds/tests failed)
(cd ports/unix && coveralls --root ../.. --build-root . --gcov $(which gcov) --gcov-options '\-o build-coverage/' --include py --include extmod)
after_failure:
(cd tests && for exp in *.exp; do testbase=$(basename $exp .exp); echo -e "\nFAILURE $testbase"; diff -u $testbase.exp $testbase.out; done)
