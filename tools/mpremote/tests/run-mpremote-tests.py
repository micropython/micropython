#! /usr/bin/env python3

import contextlib
import os
import subprocess
import sys
import sysconfig
import platform
import argparse
import inspect
import json
import re
from glob import glob
import multiprocessing
from multiprocessing.pool import ThreadPool
import threading
import tempfile
import shutil
import random

# Maximum time to run a PC-based test, in seconds.
TEST_TIMEOUT = float(os.environ.get("MICROPY_TEST_TIMEOUT", 60))

# See stackoverflow.com/questions/2632199: __file__ nor sys.argv[0]
# are guaranteed to always work, this one should though.
BASEPATH = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda: None)))


def base_path(*p):
    return os.path.abspath(os.path.join(BASEPATH, *p)).replace("\\", "/")


if os.name == "nt":
    MICROPYTHON = os.getenv(
        "MICROPY_MICROPYTHON", base_path("../../../ports/windows/build-standard/micropython.exe")
    )
else:
    MICROPYTHON = os.getenv(
        "MICROPY_MICROPYTHON", base_path("../../../ports/unix/build-standard/micropython")
    )
    SOCAT = os.getenv("MICROPY_SOCAT", "socat")

MPREMOTE = os.getenv("MICROPY_MPREMOTE", base_path("../mpremote.py"))

# File with the test results.
RESULTS_FILE = "_results.json"

# For diff'ing test output
DIFF = os.getenv("MICROPY_DIFF", "diff -u")

# Set PYTHONIOENCODING so that CPython will use utf-8 on systems which set another encoding in the locale
os.environ["PYTHONIOENCODING"] = "utf-8"


def rm_f(fname):
    if os.path.exists(fname):
        os.remove(fname)


# unescape wanted regex chars and escape unwanted ones
def convert_regex_escapes(line):
    cs = []
    escape = False
    for c in str(line, "utf8"):
        if escape:
            escape = False
            cs.append(c)
        elif c == "\\":
            escape = True
        elif c in ("(", ")", "[", "]", "{", "}", ".", "*", "+", "^", "$"):
            cs.append("\\" + c)
        else:
            cs.append(c)
    # accept carriage-return(s) before final newline
    if cs[-1] == "\n":
        cs[-1] = "\r*\n"
    return bytes("".join(cs), "utf8")


@contextlib.contextmanager
def unix_remote(mode: str = "socket"):
    with tempfile.TemporaryDirectory(prefix="micropython.") as mpdir:
        if mode == "socket":
            port = random.randint(1025, 65535)
            address = f"socket://127.0.0.1:{port}"
            socat_arg = f"TCP-LISTEN:{port},reuseaddr"
        elif mode == "pty":
            pty = os.path.join(mpdir, "pty")
            address = pty
            socat_arg = f"PTY,link={pty}"
        else:
            raise ValueError(f"unknown mode {mode}")

        try:
            server = subprocess.Popen(
                [SOCAT, socat_arg, f"SYSTEM:while true;do {MICROPYTHON};done,pty,stderr,onlcr=0"],
                cwd=mpdir,
            )
            yield address
        finally:
            server.kill()


class TestError(Exception):
    pass


class ThreadSafeCounter:
    def __init__(self, start=0):
        self._value = start
        self._lock = threading.Lock()

    def increment(self):
        self.add(1)

    def add(self, to_add):
        with self._lock:
            self._value += to_add

    def append(self, arg):
        self.add([arg])

    @property
    def value(self):
        return self._value


def run_with_mpremote(args, test_file, test_file_abspath, tmpdir, mpremote):
    try:
        return subprocess.run(
            [test_file_abspath],
            check=True,
            stdin=subprocess.DEVNULL,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            env={
                "MPREMOTE": mpremote,
                "TMP": tmpdir,
            },
            timeout=TEST_TIMEOUT,
        ).stdout
    except subprocess.CalledProcessError as e:
        return (e.stdout or b'') + "\nCRASH={}\n".format(e.returncode).encode()
    except subprocess.TimeoutExpired as e:
        return (e.stdout or b'') + "\nTIMEOUT\n".encode()


def run_test_script(args, test_file, test_file_abspath, tmpdir):
    if args.test_instance == "unix":
        with unix_remote("pty") as instance:
            mpremote = " ".join([MPREMOTE, "connect", instance])
            return run_with_mpremote(args, test_file, test_file_abspath, tmpdir, mpremote=mpremote)
    elif args.test_instance.strip():
        mpremote = " ".join([MPREMOTE, "connect", args.test_instance])
        return run_with_mpremote(args, test_file, test_file_abspath, tmpdir, mpremote=mpremote)
    else:
        return run_with_mpremote(args, test_file, test_file_abspath, tmpdir)


def run_tests(tests, args, result_dir, num_threads=1):
    testcase_count = ThreadSafeCounter()
    test_results = ThreadSafeCounter([])

    def run_one_test(test_file):
        test_file = test_file.replace("\\", "/")
        test_file_abspath = os.path.abspath(test_file).replace("\\", "/")

        if args.filters:
            # Default verdict is the opposite of the first action
            verdict = "include" if args.filters[0][0] == "exclude" else "exclude"
            for action, pat in args.filters:
                if pat.search(test_file):
                    verdict = action
            if verdict == "exclude":
                return

        test_basename = test_file.replace("..", "_").replace("./", "").replace("/", "_")
        # test_name = os.path.splitext(os.path.basename(test_file))[0]

        test_file_expected = test_file + ".exp"
        if not os.path.isfile(test_file_expected):
            raise TestError("error: test {} requires a corresponding .exp file".format(test_file))
        with open(test_file_expected, "rb") as f:
            output_expected = f.read()

        with tempfile.TemporaryDirectory(prefix="mpremote.") as tmpdir:
            # substitute the real tmpdir path in the expected output
            output_expected = output_expected.replace(b"${TMP}", tmpdir.encode())

            # Run the test on the MicroPython target.
            output_mupy = run_test_script(args, test_file, test_file_abspath, tmpdir)

        # Canonical form for all host platforms is to use \n for end-of-line.
        output_expected = output_expected.replace(b"\r\n", b"\n")
        output_mupy = output_mupy.replace(b"\r\n", b"\n")

        testcase_count.add(len(output_expected.splitlines()))
        test_passed = output_expected == output_mupy

        filename_expected = os.path.join(result_dir, test_basename + ".exp")
        filename_mupy = os.path.join(result_dir, test_basename + ".out")

        # Print test summary, update counters, and save .exp/.out files if needed.
        if test_passed:
            print("pass ", test_file)
            test_results.append((test_file, "pass", ""))
            rm_f(filename_expected)
            rm_f(filename_mupy)
        else:
            print("FAIL ", test_file)
            if output_expected is not None:
                with open(filename_expected, "wb") as f:
                    f.write(output_expected)
            else:
                rm_f(filename_expected)  # in case left over from previous failed run
            with open(filename_mupy, "wb") as f:
                f.write(output_mupy)
            test_results.append((test_file, "fail", ""))

    if args.test_instance != "unix":
        num_threads = 1

    try:
        if num_threads > 1:
            pool = ThreadPool(num_threads)
            pool.map(run_one_test, tests)
        else:
            for test in tests:
                run_one_test(test)
    except TestError as er:
        for line in er.args[0]:
            print(line)
        sys.exit(1)

    # Return test results.
    return test_results.value, testcase_count.value


# Print a summary of the results and save them to a JSON file.
# Returns True if everything succeeded, False otherwise.
def create_test_report(args, test_results, testcase_count=None):
    passed_tests = list(r for r in test_results if r[1] == "pass")
    skipped_tests = list(r for r in test_results if r[1] == "skip" and r[2] != "too large")
    skipped_tests_too_large = list(
        r for r in test_results if r[1] == "skip" and r[2] == "too large"
    )
    failed_tests = list(r for r in test_results if r[1] == "fail")

    num_tests_performed = len(passed_tests) + len(failed_tests)

    testcase_count_info = ""
    if testcase_count is not None:
        testcase_count_info = " ({} individual testcases)".format(testcase_count)
    print("{} tests performed{}".format(num_tests_performed, testcase_count_info))

    print("{} tests passed".format(len(passed_tests)))

    if len(skipped_tests) > 0:
        print(
            "{} tests skipped: {}".format(
                len(skipped_tests), " ".join(test[0] for test in skipped_tests)
            )
        )

    if len(skipped_tests_too_large) > 0:
        print(
            "{} tests skipped because they are too large: {}".format(
                len(skipped_tests_too_large), " ".join(test[0] for test in skipped_tests_too_large)
            )
        )

    if len(failed_tests) > 0:
        print(
            "{} tests failed: {}".format(
                len(failed_tests), " ".join(test[0] for test in failed_tests)
            )
        )

    # Serialize regex added by append_filter.
    def to_json(obj):
        if isinstance(obj, re.Pattern):
            return obj.pattern
        return obj

    with open(os.path.join(args.result_dir, RESULTS_FILE), "w") as f:
        json.dump(
            {
                # The arguments passed on the command-line.
                "args": vars(args),
                # A list of all results of the form [(test, result, reason), ...].
                "results": list(test for test in test_results),
                # A list of failed tests.  This is deprecated, use the "results" above instead.
                "failed_tests": [test[0] for test in failed_tests],
            },
            f,
            default=to_json,
        )

    # Return True only if all tests succeeded.
    return len(failed_tests) == 0


class append_filter(argparse.Action):
    def __init__(self, option_strings, dest, **kwargs):
        super().__init__(option_strings, dest, default=[], **kwargs)

    def __call__(self, parser, args, value, option):
        if not hasattr(args, self.dest):
            args.filters = []
        if option.startswith(("-e", "--e")):
            option = "exclude"
        else:
            option = "include"
        args.filters.append((option, re.compile(value)))


def main():
    cmd_parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description="""Run and manage mpremote tests for MicroPython.

By default the tests are run against the unix port of MicroPython. To run it
against something else, use the -t option.  See below for details.

Tests are discovered by scanning test directories for .sh files or using the
specified test files. If test files nor directories are specified, the script
expects to be ran in the tests directory (where this file is located) and the
builtin tests suitable for the target platform are ran.

When running tests, run-tests.py compares the output of the test with the interpolated
contents of the corresponding <test>.exp file.

If a test fails, run-tests.py produces a pair of <test>.out and <test>.exp files in the result
directory with the shell output and the expectations, respectively.
""",
        epilog="""\
The -t option accepts the following for the test instance:
- unix - use the unix port of MicroPython, specified by the MICROPY_MICROPYTHON
  environment variable (which defaults to the standard variant of either the unix
  or windows ports, depending on the host platform)
- port:<device> - connect to and use the given serial port device
- a<n> - connect to and use /dev/ttyACM<n>
- u<n> - connect to and use /dev/ttyUSB<n>
- c<n> - connect to and use COM<n>
- anything else specifies a serial port

Options -i and -e can be multiple and processed in the order given. Regex
"search" (vs "match") operation is used. An action (include/exclude) of
the last matching regex is used:
  run-tests.py -i async - exclude all, then include tests containing "async" anywhere
  run-tests.py -e '/big.+int' - include all, then exclude by regex
  run-tests.py -e async -i async_foo - include all, exclude async, yet still include async_foo
""",
    )
    cmd_parser.add_argument(
        "-t", "--test-instance", default="unix", help="the MicroPython instance to test"
    )
    cmd_parser.add_argument(
        "-d", "--test-dirs", nargs="*", help="input test directories (if no files given)"
    )
    cmd_parser.add_argument(
        "-r", "--result-dir", default=base_path("results"), help="directory for test results"
    )
    cmd_parser.add_argument(
        "-e",
        "--exclude",
        action=append_filter,
        metavar="REGEX",
        dest="filters",
        help="exclude test by regex on path/name.py",
    )
    cmd_parser.add_argument(
        "-i",
        "--include",
        action=append_filter,
        metavar="REGEX",
        dest="filters",
        help="include test by regex on path/name.py",
    )
    cmd_parser.add_argument(
        "-j",
        "--jobs",
        default=multiprocessing.cpu_count(),
        metavar="N",
        type=int,
        help="Number of tests to run simultaneously",
    )
    cmd_parser.add_argument("files", nargs="*", help="input test files")
    cmd_parser.add_argument(
        "--print-failures",
        action="store_true",
        help="print the diff of expected vs. actual output for failed tests and exit",
    )
    cmd_parser.add_argument(
        "--clean-failures",
        action="store_true",
        help="delete the .exp and .out files from failed tests and exit",
    )
    cmd_parser.add_argument(
        "--run-failures",
        action="store_true",
        help="re-run only the failed tests",
    )
    args = cmd_parser.parse_args()

    if args.print_failures:
        for out in glob(os.path.join(args.result_dir, "*.out")):
            testbase = out[:-4]
            print()
            print("FAILURE {0}".format(testbase))
            if os.path.exists(testbase + ".exp"):
                # Show diff of expected and actual output.
                os.system("{0} {1}.exp {1}.out".format(DIFF, testbase))
            else:
                # No expected output, just show the actual output (eg from a unittest).
                with open(out) as f:
                    for line in f:
                        print(line, end="")

        sys.exit(0)

    if args.clean_failures:
        for f in glob(os.path.join(args.result_dir, "*.exp")) + glob(
            os.path.join(args.result_dir, "*.out")
        ):
            os.remove(f)
        rm_f(os.path.join(args.result_dir, RESULTS_FILE))

        sys.exit(0)

    if args.run_failures and (any(args.files) or args.test_dirs is not None):
        raise ValueError(
            "--run-failures cannot be used together with files or --test-dirs arguments"
        )

    if args.run_failures:
        results_file = os.path.join(args.result_dir, RESULTS_FILE)
        if os.path.exists(results_file):
            with open(results_file, "r") as f:
                tests = list(test[0] for test in json.load(f)["results"] if test[1] == "fail")
        else:
            tests = []
    elif len(args.files) == 0:
        if sys.platform == "nt":
            test_extensions = ("test_*.ps1",)
        else:
            test_extensions = ("test_*.sh", "test_*.ps1")

        if args.test_dirs is None:
            test_dirs = (".",)
        else:
            test_dirs = args.test_dirs

        tests = sorted(
            test_file
            for test_files in (
                glob(os.path.join(dir, ext)) for dir in test_dirs for ext in test_extensions
            )
            for test_file in test_files
        )
    else:
        # tests explicitly given
        tests = args.files

    os.makedirs(args.result_dir, exist_ok=True)
    test_results, testcase_count = run_tests(tests, args, args.result_dir, args.jobs)
    res = create_test_report(args, test_results, testcase_count)

    if not res:
        sys.exit(1)


if __name__ == "__main__":
    main()
