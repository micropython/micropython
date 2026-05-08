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
