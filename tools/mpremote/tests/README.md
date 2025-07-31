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

Each test should print "OK" if it passed.  Otherwise it will print "CRASH", or "FAIL"
and a diff of the expected and actual test output.
