#! /usr/bin/env python3

import os
import subprocess
import sys
import argparse
import re
from glob import glob
from collections import defaultdict

# Tests require at least CPython 3.3. If your default python3 executable
# is of lower version, you can point MICROPY_CPYTHON3 environment var
# to the correct executable.
if os.name == "nt":
    CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3.exe")
    MICROPYTHON = os.getenv("MICROPY_MICROPYTHON", "../ports/windows/micropython.exe")
else:
    CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3")
    MICROPYTHON = os.getenv("MICROPY_MICROPYTHON", "../ports/unix/micropython")


def run_tests(pyb, test_dict):
    test_count = 0
    testcase_count = 0

    for base_test, tests in sorted(test_dict.items()):
        print(base_test + ":")
        for test_file in tests:

            # run MicroPython
            if pyb is None:
                # run on PC
                try:
                    output_mupy = subprocess.check_output(
                        [MICROPYTHON, "-X", "emit=bytecode", test_file[0]]
                    )
                except subprocess.CalledProcessError:
                    output_mupy = b"CRASH"
            else:
                # run on pyboard
                pyb.enter_raw_repl()
                try:
                    output_mupy = pyb.execfile(test_file).replace(b"\r\n", b"\n")
                except pyboard.PyboardError:
                    output_mupy = b"CRASH"

            output_mupy = float(output_mupy.strip())
            test_file[1] = output_mupy
            testcase_count += 1

        test_count += 1
        baseline = None
        for t in tests:
            if baseline is None:
                baseline = t[1]
            print("    %.3fs (%+06.2f%%) %s" % (t[1], (t[1] * 100 / baseline) - 100, t[0]))

    print("{} tests performed ({} individual testcases)".format(test_count, testcase_count))

    # all tests succeeded
    return True


def main():
    cmd_parser = argparse.ArgumentParser(description="Run tests for MicroPython.")
    cmd_parser.add_argument("--pyboard", action="store_true", help="run the tests on the pyboard")
    cmd_parser.add_argument("files", nargs="*", help="input test files")
    args = cmd_parser.parse_args()

    # Note pyboard support is copied over from run-tests, not testes, and likely needs revamping
    if args.pyboard:
        import pyboard

        pyb = pyboard.Pyboard("/dev/ttyACM0")
        pyb.enter_raw_repl()
    else:
        pyb = None

    if len(args.files) == 0:
        if pyb is None:
            # run PC tests
            test_dirs = ("internal_bench",)
        else:
            # run pyboard tests
            test_dirs = ("basics", "float", "pyb")
        tests = sorted(
            test_file
            for test_files in (glob("{}/*.py".format(dir)) for dir in test_dirs)
            for test_file in test_files
        )
    else:
        # tests explicitly given
        tests = sorted(args.files)

    test_dict = defaultdict(lambda: [])
    for t in tests:
        m = re.match(r"(.+?)-(.+)\.py", t)
        if not m:
            continue
        test_dict[m.group(1)].append([t, None])

    if not run_tests(pyb, test_dict):
        sys.exit(1)


if __name__ == "__main__":
    main()
