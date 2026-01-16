#! /usr/bin/env python3

import os
import subprocess
import sys
import argparse
import re
from glob import glob
from collections import defaultdict

run_tests_module = __import__("run-tests")
sys.path.append(run_tests_module.base_path("../tools"))
import pyboard

if os.name == "nt":
    MICROPYTHON = os.getenv(
        "MICROPY_MICROPYTHON", "../ports/windows/build-standard/micropython.exe"
    )
    CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3")
else:
    MICROPYTHON = os.getenv("MICROPY_MICROPYTHON", "../ports/unix/build-standard/micropython")
    CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3")

MICROPYTHON_CMD = [MICROPYTHON, "-X", "emit=bytecode"]
CPYTHON3_CMD = [CPYTHON3, "-BS"]


injected_bench_code = b"""
import time

class bench_class:
    ITERS = 20000000

    @staticmethod
    def run(test):
        t = time.ticks_us()
        test(bench_class.ITERS)
        t = time.ticks_diff(time.ticks_us(), t)
        s, us = divmod(t, 1_000_000)
        print("{}.{:06}".format(s, us))

import sys
sys.modules['bench'] = bench_class
"""


def execbench(test_instance, filename, iters):
    with open(filename, "rb") as f:
        pyfile = f.read()
    code = (injected_bench_code + pyfile).replace(b"20000000", str(iters).encode("utf-8"))
    return test_instance.exec(code).replace(b"\r\n", b"\n")


def run_tests(test_instance, test_dict, iters):
    test_count = 0
    testcase_count = 0

    for base_test, tests in sorted(test_dict.items()):
        print(base_test + ":")
        baseline = None
        for test_file in tests:
            # run MicroPython
            if isinstance(test_instance, list):
                # run on PC
                try:
                    output_mupy = subprocess.check_output(test_instance + [test_file[0]])
                except subprocess.CalledProcessError:
                    output_mupy = b"CRASH"
            else:
                # run on pyboard
                test_instance.enter_raw_repl()
                try:
                    output_mupy = execbench(test_instance, test_file[0], iters)
                except pyboard.PyboardError:
                    output_mupy = b"CRASH"

            try:
                output_mupy = float(output_mupy.strip())
            except ValueError:
                output_mupy = -1
            test_file[1] = output_mupy
            testcase_count += 1

            if baseline is None:
                baseline = test_file[1]
            print(
                "    %.3fs (%+06.2f%%) %s"
                % (test_file[1], (test_file[1] * 100 / baseline) - 100, test_file[0])
            )

        test_count += 1

    print("{} tests performed ({} individual testcases)".format(test_count, testcase_count))

    # all tests succeeded
    return True


def main():
    cmd_parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=f"""Run and manage tests for MicroPython.

{run_tests_module.test_instance_description}
{run_tests_module.test_directory_description}
""",
        epilog=f"""{run_tests_module.test_instance_epilog}- cpython - use CPython to run the benchmarks instead\n""",
    )
    cmd_parser.add_argument(
        "-t", "--test-instance", default="unix", help="the MicroPython instance to test"
    )
    cmd_parser.add_argument(
        "-b", "--baudrate", default=115200, help="the baud rate of the serial device"
    )
    cmd_parser.add_argument("-u", "--user", default="micro", help="the telnet login username")
    cmd_parser.add_argument("-p", "--password", default="python", help="the telnet login password")
    cmd_parser.add_argument(
        "-d", "--test-dirs", nargs="*", help="input test directories (if no files given)"
    )
    cmd_parser.add_argument(
        "-I",
        "--iters",
        type=int,
        default=200_000,
        help="number of test iterations, only for remote instances (default 200,000)",
    )
    cmd_parser.add_argument("files", nargs="*", help="input test files")
    args = cmd_parser.parse_args()

    if args.test_instance == "cpython":
        test_instance = CPYTHON3_CMD
    else:
        # Note pyboard support is copied over from run-tests.py, not tests, and likely needs revamping
        test_instance = run_tests_module.get_test_instance(
            args.test_instance, args.baudrate, args.user, args.password
        )
        if test_instance is None:
            test_instance = MICROPYTHON_CMD

    if len(args.files) == 0:
        if args.test_dirs:
            test_dirs = tuple(args.test_dirs)
        else:
            test_dirs = ("internal_bench",)

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

    if not run_tests(test_instance, test_dict, args.iters):
        sys.exit(1)


if __name__ == "__main__":
    main()
