# Tests for mpremote

This directory contains a set of tests for `mpremote`.

Requirements:
- A device running MicroPython connected to a serial port on the host.
- The device you are testing against must be flashed with a firmware of the same build
  as `mpremote`.
- If the device has an SDcard or other vfs mounted, the vfs's filesystem must be empty
  to pass the filesystem test.
- Python 3.x, `bash` and various Unix tools such as `find`, `mktemp`, `sed`, `sort`, `tr`.
- To test on Windows, you can either:
    - Run the (Linux) tests in WSL2 against a USB device that is passed though to WSL2.
    - Use the `Git Bash` terminal to run the tests against a device connected to a COM
      port. _Note:_ While the tests will run in `Git Bash`, several will throw false
      positive errors due to differences in the way that TMP files are logged and and
      several other details.

To run the tests do:

    $ cd tools/mpremote/tests
    $ ./run-mpremote-tests.sh

To run a single test do:

    $ ./run-mpremote-tests.sh test_filesystem.sh

To run tests against a specific device:

    $ ./run-mpremote-tests.sh -t /dev/ttyACM0
    $ ./run-mpremote-tests.sh -t rfc2217://localhost:2217

Device shortcuts are supported: `a0` → `/dev/ttyACM0`, `u0` → `/dev/ttyUSB0`, `c0` → `COM0`.

Each test should print "OK" if it passed.  Otherwise it will print "CRASH", "FAIL"
(with a diff of expected vs actual output), or "skip" if the test was skipped.

## Skipping Tests

Tests can skip themselves by outputting `SKIP` on a line by itself and exiting with
code 0. This is useful when a test is not applicable to a particular device or
configuration.

Example in a test script:
```bash
if [[ "${MPREMOTE_DEVICE}" == rfc2217://* ]]; then
    echo "SKIP"
    exit 0
fi
```

The `MPREMOTE_DEVICE` environment variable contains the device path passed via `-t`.

## Using the RAM Disk

Tests that need a writable filesystem can use `ramdisk.py` to create a temporary
FAT filesystem in RAM:

```bash
TEST_DIR=$(dirname $0)
$MPREMOTE run "${TEST_DIR}/ramdisk.py"
```
Defaults to: block_size=512, num_blocks=50, mount_path='/ramdisk', do_chdir=True
Custom parameters can be passed via `exec`:

```bash
$MPREMOTE exec "mount_path='/__ramdisk'; do_chdir=False" run "${TEST_DIR}/ramdisk.py"
```
## Running with Coverage

Both bash tests and pytest tests can collect coverage data. To get combined coverage
from both test suites:

```bash
cd tools/mpremote

# 1. Start fresh
coverage erase

# 2. Run bash tests with coverage (creates parallel data files)
./tests/run-mpremote-tests.sh -c

# 3. Combine bash test coverage into .coverage
coverage combine

# 4. Run pytest (appends to existing coverage data)
MPREMOTE_DEVICES="socket://localhost:2218,rfc2217://localhost:2217" pytest tests/

# 5. View reports
coverage report          # Terminal summary
coverage html            # HTML report in htmlcov/
```

**Important:** Run bash tests first, then `coverage combine`, then pytest. The pytest
configuration includes `--cov-append` which adds to existing coverage data rather than
replacing it.

The HTML report shows which tests covered each line, including scenario contexts
(e.g., `[socket]`, `[rfc2217]`) for device-specific coverage tracking.

### Coverage Options

- Pytest automatically collects coverage with append mode (configured in `pyproject.toml`)
- Bash tests: use `-c` flag to enable coverage
- Device shortcuts work with coverage: `./tests/run-mpremote-tests.sh -t a0 -c`

### Subprocess Coverage

Both test frameworks collect coverage from subprocesses (the actual `mpremote` commands
being tested), not just from the test harness code.

**Bash tests:** When using `-c`, the script wraps each `mpremote` invocation with
`coverage run --context=<test_name>`. This creates parallel coverage data files
(`.coverage.<hostname>.<pid>.<random>`) that are later combined with `coverage combine`.

**Pytest tests:** The `conftest.py` sets up subprocess coverage by:
1. Setting `COVERAGE_PROCESS_START` to point to `pyproject.toml`
2. Adding the tests directory to `PYTHONPATH` so `sitecustomize.py` is discovered
3. Passing the test context via `MPREMOTE_COVERAGE_CONTEXT` environment variable

When a subprocess starts, Python automatically imports `sitecustomize.py`, which starts
coverage collection with the test's context. This allows tracking which device scenarios
(socket, rfc2217, serial) covered each line of code.

The coverage configuration in `pyproject.toml` uses `parallel = true` to support
collecting data from multiple subprocesses without conflicts.

