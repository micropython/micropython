# MicroPython Test Suite

This directory contains the test suite for MicroPython itself - the
interpreter, runtime and standard library. Tests are run with the runner
scripts in this directory, most commonly `run-tests.py`.

Full documentation, including how to write tests and advanced runner options,
is at https://docs.micropython.org/en/latest/develop/writingtests.html

## Running tests

```bash
cd tests/

# All tests on the unix port (default)
./run-tests.py

# On a microcontroller via serial
./run-tests.py -t /dev/ttyACM0    # Linux
./run-tests.py -t COM3            # Windows
./run-tests.py -t a0              # Shortcut for /dev/ttyACM0

# Specific tests or directories
./run-tests.py basics/int_parse.py
./run-tests.py float/float*.py
./run-tests.py -d basics -d float
```

On Windows use `python run-tests.py`.

## Test results

```
pass  basics/array1.py
skip  basics/async_await.py
FAIL  basics/builtin_abs.py
```

When tests fail, the actual and expected outputs are written to the
`results/` directory:

```bash
./run-tests.py --print-failures    # show diffs of failed tests
./run-tests.py --run-failures      # re-run only the failed tests
./run-tests.py --clean-failures    # delete previous results
```

## Other test runners

- `run-multitests.py` - tests needing multiple MicroPython instances
  (networking, Bluetooth)
- `run-perfbench.py` - performance benchmarks
- `run-natmodtests.py` - dynamic native module tests
- `run-internalbench.py` - low-level VM/C benchmarks

See the [documentation](https://docs.micropython.org/en/latest/develop/writingtests.html)
for usage of each, and for extra setup needed by some tests (e.g. SSL/TLS
certificates for network tests).

## Writing tests

Tests are Python scripts whose output is compared against CPython running
the same script, or against a `<test>.py.exp` expected-output file. Tests
for MicroPython-only functionality are preferably written using `unittest`.
See the [documentation](https://docs.micropython.org/en/latest/develop/writingtests.html)
for details.

## Reporting failures

When reporting test failures, include the failing test names and diffs
(`./run-tests.py --print-failures`), the MicroPython version
(`micropython -c "import sys; print(sys.version)"`), and the board/port
used.
