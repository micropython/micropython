# Tests for mpremote

This directory contains a set of tests for `mpremote`.

Requirements:
- A device running MicroPython connected to a serial port on the host.
- Python 3.x, `bash` and various Unix tools such as `find`, `mktemp`, `sed`, `sort`, `tr`.

To run the tests do:

    $ ./run-mpremote-tests.sh

Each test should print "OK" if it passed.  Otherwise it will print "CRASH", or "FAIL"
and a diff of the expected and actual test output.
