# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2019-2025 Damien P. George

import inspect
import json
import os
import re
import subprocess
import sys
import tempfile

# See stackoverflow.com/questions/2632199: __file__ nor sys.argv[0]
# are guaranteed to always work, this one should though.
_BASEPATH = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda: None)))


def base_path(*p):
    return os.path.abspath(os.path.join(_BASEPATH, *p)).replace("\\", "/")


sys.path.append(base_path("../tools"))
import pyboard

# Prefix used by run-tests.py to tag known-flaky test results.
FLAKY_REASON_PREFIX = "flaky"

# File with the test results.
_RESULTS_FILE = "_results.json"

# Maximum time to run a single test, in seconds.
TEST_TIMEOUT = float(os.environ.get("MICROPY_TEST_TIMEOUT", 30))

# Maximum time to wait to enter the raw REPL at the start of a test, in seconds.
TEST_ENTER_RAW_REPL_TIMEOUT = 5

# Maximum number of raw REPL failures to tolerate before aborting the entire test run.
TEST_MAXIMUM_RAW_REPL_FAILURES = 3

# mpy-cross is only needed if --via-mpy command-line arg is passed
if os.name == "nt":
    MPYCROSS = os.getenv("MICROPY_MPYCROSS", base_path("../mpy-cross/build/mpy-cross.exe"))
else:
    MPYCROSS = os.getenv("MICROPY_MPYCROSS", base_path("../mpy-cross/build/mpy-cross"))

test_instance_description = """\
By default the tests are run against the unix port of MicroPython. To run it
against something else, use the -t option.  See below for details.
"""

test_instance_epilog = """\
The -t option accepts the following for the test instance:
- unix - use the unix port of MicroPython, specified by the MICROPY_MICROPYTHON
  environment variable (which defaults to the standard variant of either the unix
  or windows ports, depending on the host platform)
- webassembly - use the webassembly port of MicroPython, specified by the
  MICROPY_MICROPYTHON_MJS environment variable (which defaults to the standard
  variant of the webassembly port)
- port:<device> - connect to and use the given serial port device
- a<n> - connect to and use /dev/ttyACM<n>
- u<n> - connect to and use /dev/ttyUSB<n>
- c<n> - connect to and use COM<n>
- exec:<command> - execute a command and attach to its stdin/stdout
- execpty:<command> - execute a command and attach to the printed /dev/pts/<n> device
- <a>.<b>.<c>.<d> - connect to the given IPv4 address
- anything else specifies a serial port
"""

test_directory_description = """\
Tests are discovered by scanning test directories for .py files or using the
specified test files. If test files nor directories are specified, the script
expects to be ran in the tests directory (where this file is located) and the
builtin tests suitable for the target platform are ran.
"""

# Code to allow a target MicroPython to import an .mpy from RAM
# Note: the module is named `__injected_test` but it needs to have `__name__` set to
# `__main__` so that the test sees itself as the main module, eg so unittest works.
_injected_import_hook_code = """\
import sys, os, io, vfs
class __File(io.IOBase):
  def __init__(self):
    module = sys.modules['__injected_test']
    module.__name__ = '__main__'
    sys.modules['__main__'] = module
    self.off = 0
  def ioctl(self, request, arg):
    if request == 4: # MP_STREAM_CLOSE
      return 0
    return -1
  def readinto(self, buf):
    buf[:] = memoryview(__buf)[self.off:self.off + len(buf)]
    self.off += len(buf)
    return len(buf)
class __FS:
  def mount(self, readonly, mkfs):
    pass
  def umount(self):
    pass
  def chdir(self, path):
    pass
  def getcwd(self):
    return ""
  def stat(self, path):
    if path == '__injected_test.mpy':
      return (0,0,0,0,0,0,0,0,0,0)
    else:
      raise OSError(2) # ENOENT
  def open(self, path, mode):
    self.stat(path)
    return __File()
vfs.mount(__FS(), '/__vfstest')
os.chdir('/__vfstest')
{import_prologue}
__import__('__injected_test')
"""


class PyboardNodeRunner:
    def __init__(self):
        mjs = os.getenv("MICROPY_MICROPYTHON_MJS")
        if mjs is None:
            mjs = base_path("../ports/webassembly/build-standard/micropython.mjs")
        else:
            mjs = os.path.abspath(mjs)
        self.micropython_mjs = mjs

    def close(self):
        pass

    def run_script_on_remote_target(self, args, test_file, is_special, requires_target_wiring):
        cwd = os.path.dirname(test_file)

        # Create system command list.
        cmdlist = ["node"]
        if test_file.endswith(".py"):
            # Run a Python script indirectly via "node micropython.mjs <script.py>".
            cmdlist.append(self.micropython_mjs)
            if args.heapsize is not None:
                cmdlist.extend(["-X", "heapsize=" + args.heapsize])
            cmdlist.append(test_file)
        else:
            # Run a js/mjs script directly with Node, passing in the path to micropython.mjs.
            cmdlist.append(test_file)
            cmdlist.append(self.micropython_mjs)

        # Run the script.
        try:
            had_crash = False
            output_mupy = subprocess.check_output(
                cmdlist, stderr=subprocess.STDOUT, timeout=TEST_TIMEOUT, cwd=cwd
            )
        except subprocess.CalledProcessError as er:
            had_crash = True
            output_mupy = er.output + b"CRASH"
        except subprocess.TimeoutExpired as er:
            had_crash = True
            output_mupy = (er.output or b"") + b"TIMEOUT"

        # Return the results.
        return had_crash, output_mupy


def rm_f(fname):
    if os.path.exists(fname):
        os.remove(fname)


def normalize_newlines(data):
    """Normalize newline variations to \\n.

    Only normalizes actual line endings, not literal \\r characters in strings.
    Handles \\r\\r\\n and \\r\\n cases to ensure consistent comparison
    across different platforms and terminals.
    """
    if isinstance(data, bytes):
        # Handle PTY double-newline issue first
        data = data.replace(b"\r\r\n", b"\n")
        # Then handle standard Windows line endings
        data = data.replace(b"\r\n", b"\n")
        # Don't convert standalone \r as it might be literal content
    return data


def set_injected_prologue(prologue):
    global _injected_import_hook_code
    _injected_import_hook_code = _injected_import_hook_code.replace("{import_prologue}", prologue)


def get_results_filename(args):
    return os.path.join(args.result_dir, _RESULTS_FILE)


def convert_device_shortcut_to_real_device(device):
    if device.startswith("port:"):
        return device.split(":", 1)[1]
    elif device.startswith("a") and device[1:].isdigit():
        return "/dev/ttyACM" + device[1:]
    elif device.startswith("u") and device[1:].isdigit():
        return "/dev/ttyUSB" + device[1:]
    elif device.startswith("c") and device[1:].isdigit():
        return "COM" + device[1:]
    else:
        return device


def get_test_instance(test_instance, baudrate, user, password):
    if test_instance == "unix":
        return None
    elif test_instance == "webassembly":
        return PyboardNodeRunner()
    else:
        # Assume it's a device path.
        port = convert_device_shortcut_to_real_device(test_instance)

    pyb = pyboard.Pyboard(port, baudrate, user, password)
    pyboard.Pyboard.run_script_on_remote_target = run_script_on_remote_target
    try:
        pyb.enter_raw_repl()
    except pyboard.PyboardError as e:
        print("error: could not detect test instance")
        print(e)
        sys.exit(2)
    return pyb


def prepare_script_for_target(args, *, script_text=None, force_plain=False):
    if force_plain or (not args.via_mpy and args.emit == "bytecode"):
        # A plain test to run as-is, no processing needed.
        pass
    elif args.via_mpy:
        tempname = tempfile.mktemp(dir="")
        mpy_filename = tempname + ".mpy"

        script_filename = tempname + ".py"
        with open(script_filename, "wb") as f:
            f.write(script_text)

        try:
            subprocess.check_output(
                [MPYCROSS]
                + args.mpy_cross_flags.split()
                + ["-o", mpy_filename, "-X", "emit=" + args.emit, script_filename],
                stderr=subprocess.STDOUT,
            )
        except subprocess.CalledProcessError as er:
            return True, b"mpy-cross crash\n" + er.output

        with open(mpy_filename, "rb") as f:
            script_text = b"__buf=" + bytes(repr(f.read()), "ascii") + b"\n"

        rm_f(mpy_filename)
        rm_f(script_filename)

        script_text += bytes(_injected_import_hook_code, "ascii")
    else:
        print("error: using emit={} must go via .mpy".format(args.emit))
        sys.exit(1)

    return False, script_text


def run_script_on_remote_target(pyb, args, test_file, is_special, requires_target_wiring):
    with open(test_file, "rb") as f:
        script = f.read()

    # If the test is not a special test, prepend it with a print to indicate that it started.
    # If the print does not execute this means that the test did not even start, eg it was
    # too large for the target.
    prepend_start_test = not is_special
    if prepend_start_test:
        if script.startswith(b"#"):
            script = b"print('START TEST')" + script
        else:
            script = b"print('START TEST')\n" + script

    had_crash, script = prepare_script_for_target(args, script_text=script, force_plain=is_special)

    if had_crash:
        return True, script

    try:
        pyb.enter_raw_repl(timeout_overall=TEST_ENTER_RAW_REPL_TIMEOUT)
        if requires_target_wiring and pyb.target_wiring_script:
            pyb.exec_(
                "import sys;sys.modules['target_wiring']=__build_class__(lambda:exec("
                + repr(pyb.target_wiring_script)
                + "),'target_wiring')"
            )
        output_mupy = pyb.exec_(script, timeout=TEST_TIMEOUT)
    except pyboard.PyboardError as e:
        had_crash = True
        if not is_special and e.args[0] == "exception":
            if prepend_start_test and e.args[1] == b"" and b"MemoryError" in e.args[2]:
                output_mupy = b"SKIP-TOO-LARGE\n"
            else:
                output_mupy = e.args[1] + e.args[2] + b"CRASH"
        else:
            output_mupy = bytes(e.args[0], "ascii") + b"\nCRASH"

    if prepend_start_test:
        if output_mupy.startswith(b"START TEST\r\n"):
            output_mupy = output_mupy.removeprefix(b"START TEST\r\n")
        else:
            had_crash = True

    return had_crash, output_mupy


# Print a summary of the results and save them to a JSON file.
# Returns True if everything succeeded, False otherwise.
def create_test_report(args, test_results, testcase_count=None):
    passed_tests = list(r for r in test_results if r[1] == "pass")
    skipped_tests = list(r for r in test_results if r[1] == "skip" and r[2] != "too large")
    skipped_tests_too_large = list(
        r for r in test_results if r[1] == "skip" and r[2] == "too large"
    )
    failed_tests = list(r for r in test_results if r[1] == "fail")
    ignored_tests = list(r for r in test_results if r[1] == "ignored")

    num_tests_performed = len(passed_tests) + len(failed_tests) + len(ignored_tests)

    testcase_count_info = ""
    if testcase_count is not None:
        testcase_count_info = " ({} individual testcases)".format(testcase_count)
    print("{} tests performed{}".format(num_tests_performed, testcase_count_info))

    print("{} tests passed".format(len(passed_tests)))

    if len(ignored_tests) > 0:
        print(
            "{} tests had known-flaky failures (ignored): {}".format(
                len(ignored_tests),
                " ".join("{} [{}]".format(t[0], t[2]) for t in ignored_tests),
            )
        )

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

    with open(get_results_filename(args), "w") as f:
        json.dump(
            {
                # The arguments passed on the command-line.
                "args": vars(args),
                # A list of all results of the form [(test, result, reason), ...].
                "results": list(test for test in test_results),
                # A list of failed tests.  This is deprecated, use the "results" above instead.
                "failed_tests": [test[0] for test in failed_tests],
                # A list of known-flaky tests whose failures were ignored.
                "ignored_tests": [test[0] for test in ignored_tests],
            },
            f,
            default=to_json,
        )

    # Return True only if all tests succeeded.
    return len(failed_tests) == 0
