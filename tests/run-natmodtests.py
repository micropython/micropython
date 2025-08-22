#!/usr/bin/env python3

# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2019 Damien P. George

import os
import subprocess
import sys
import argparse

run_tests_module = __import__("run-tests")

# Paths for host executables
CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3")
MICROPYTHON = os.getenv("MICROPY_MICROPYTHON", "../ports/unix/build-coverage/micropython")

NATMOD_EXAMPLE_DIR = "../examples/natmod/"

# Supported tests and their corresponding mpy module
TEST_MAPPINGS = {
    "btree": "btree/btree_$(ARCH).mpy",
    "deflate": "deflate/deflate_$(ARCH).mpy",
    "framebuf": "framebuf/framebuf_$(ARCH).mpy",
    "heapq": "heapq/heapq_$(ARCH).mpy",
    "random": "random/random_$(ARCH).mpy",
    "re": "re/re_$(ARCH).mpy",
}

# Supported architectures for native mpy modules
AVAILABLE_ARCHS = (
    "x86",
    "x64",
    "armv6",
    "armv6m",
    "armv7m",
    "armv7em",
    "armv7emsp",
    "armv7emdp",
    "xtensa",
    "xtensawin",
    "rv32imc",
)

ARCH_MAPPINGS = {"armv7em": "armv7m"}

# Code to allow a target MicroPython to import an .mpy from RAM
injected_import_hook_code = """\
import sys, io, vfs
class __File(io.IOBase):
  def __init__(self):
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
  def chdir(self, path):
    pass
  def stat(self, path):
    if path == '/__injected.mpy':
      return tuple(0 for _ in range(10))
    else:
      raise OSError(-2) # ENOENT
  def open(self, path, mode):
    return __File()
vfs.mount(__FS(), '/__remote')
sys.path.insert(0, '/__remote')
{import_prelude}
sys.modules['{}'] = __import__('__injected')
"""


class TargetSubprocess:
    def __init__(self, cmd):
        self.cmd = cmd

    def close(self):
        pass

    def run_script(self, script):
        try:
            p = subprocess.run(
                self.cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, input=script
            )
            return p.stdout, None
        except subprocess.CalledProcessError as er:
            return b"", er


class TargetPyboard:
    def __init__(self, pyb):
        self.pyb = pyb
        self.pyb.enter_raw_repl()

    def close(self):
        self.pyb.exit_raw_repl()
        self.pyb.close()

    def run_script(self, script):
        try:
            self.pyb.enter_raw_repl()
            output = self.pyb.exec_(script)
            output = output.replace(b"\r\n", b"\n")
            return output, None
        except run_tests_module.pyboard.PyboardError as er:
            return b"", er


def detect_architecture(target):
    with open("./feature_check/target_info.py", "rb") as f:
        target_info_data = f.read()
        result_out, error = target.run_script(target_info_data)
        if error is not None:
            return None, None, error
        info = result_out.split(b" ")
        if len(info) < 2:
            return None, None, "unexpected target info: {}".format(info)
        platform = info[0].strip().decode()
        arch = info[1].strip().decode()
        if arch not in AVAILABLE_ARCHS:
            if arch == "None":
                return None, None, "the target does not support dynamic modules"
            else:
                return None, None, "{} is not a supported architecture".format(arch)
        return platform, arch, None


def run_tests(target_truth, target, args, resolved_arch):
    global injected_import_hook_code

    prelude = ""
    if args.begin:
        prelude = args.begin.read()
    injected_import_hook_code = injected_import_hook_code.replace("{import_prelude}", prelude)

    test_results = []
    for test_file in args.files:
        # Find supported test
        test_file_basename = os.path.basename(test_file)
        for k, v in TEST_MAPPINGS.items():
            if test_file_basename.startswith(k):
                test_module = k
                test_mpy = v.replace("$(ARCH)", resolved_arch)
                break
        else:
            print("----  {} - no matching mpy".format(test_file))
            continue

        # Read test script
        with open(test_file, "rb") as f:
            test_file_data = f.read()

        # Create full test with embedded .mpy
        test_script = b"import sys\nsys.path.remove('')\n\n"
        try:
            with open(NATMOD_EXAMPLE_DIR + test_mpy, "rb") as f:
                test_script += b"__buf=" + bytes(repr(f.read()), "ascii") + b"\n"
        except OSError:
            test_results.append((test_file, "skip", "mpy file not compiled"))
            print("skip  {} - mpy file not compiled".format(test_file))
            continue
        test_script += bytes(injected_import_hook_code.format(test_module), "ascii")
        test_script += b"print('START TEST')\n"
        test_script += test_file_data

        # Run test under MicroPython
        result_out, error = target.run_script(test_script)

        # Work out result of test
        extra = ""
        result_out = result_out.removeprefix(b"START TEST\n")
        if error is None and result_out == b"SKIP\n":
            result = "SKIP"
        elif (
            error is not None
            and error.args[0] == "exception"
            and error.args[1] == b""
            and b"MemoryError" in error.args[2]
        ):
            # Test had a MemoryError before anything (should be at least "START TEST")
            # was printed, so the test is too big for the target.
            result = "LRGE"
        elif error is not None:
            result = "FAIL"
            extra = " - " + str(error)
        else:
            # Check result against truth
            try:
                with open(test_file + ".exp", "rb") as f:
                    result_exp = f.read()
                error = None
            except OSError:
                result_exp, error = target_truth.run_script(test_file_data)
            if error is not None:
                result = "TRUTH FAIL"
            elif result_out != result_exp:
                result = "FAIL"
                print(result_out)
            else:
                result = "pass"

        # Accumulate statistics
        if result == "pass":
            test_results.append((test_file, "pass", ""))
        elif result == "SKIP":
            test_results.append((test_file, "skip", ""))
        elif result == "LRGE":
            test_results.append((test_file, "skip", "too large"))
        else:
            test_results.append((test_file, "fail", ""))

        # Print result
        print("{:4}  {}{}".format(result, test_file, extra))

    return test_results


def main():
    cmd_parser = argparse.ArgumentParser(
        description="Run dynamic-native-module tests under MicroPython",
        epilog=run_tests_module.test_instance_epilog,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    cmd_parser.add_argument(
        "-t", "--test-instance", default="unix", help="the MicroPython instance to test"
    )
    cmd_parser.add_argument("--baudrate", default=115200, help="baud rate of the serial device")
    cmd_parser.add_argument("--user", default="micro", help="telnet login username")
    cmd_parser.add_argument("--password", default="python", help="telnet login password")
    cmd_parser.add_argument(
        "-a", "--arch", choices=AVAILABLE_ARCHS, help="override native architecture of the target"
    )
    cmd_parser.add_argument(
        "-b",
        "--begin",
        type=argparse.FileType("rt"),
        default=None,
        help="prologue python file to execute before module import",
    )
    cmd_parser.add_argument(
        "-r",
        "--result-dir",
        default=run_tests_module.base_path("results"),
        help="directory for test results",
    )
    cmd_parser.add_argument("files", nargs="*", help="input test files")
    args = cmd_parser.parse_args()

    target_truth = TargetSubprocess([CPYTHON3])

    target = run_tests_module.get_test_instance(
        args.test_instance, args.baudrate, args.user, args.password
    )
    if target is None:
        # Use the unix port of MicroPython.
        target = TargetSubprocess([MICROPYTHON])
    else:
        # Use a remote target.
        target = TargetPyboard(target)

    if hasattr(args, "arch") and args.arch is not None:
        target_arch = args.arch
        target_platform = None
    else:
        target_platform, target_arch, error = detect_architecture(target)
        if error:
            print("Cannot run tests: {}".format(error))
            sys.exit(1)
    target_arch = ARCH_MAPPINGS.get(target_arch, target_arch)

    if target_platform:
        print("platform={} ".format(target_platform), end="")
    print("arch={}".format(target_arch))

    os.makedirs(args.result_dir, exist_ok=True)
    test_results = run_tests(target_truth, target, args, target_arch)
    res = run_tests_module.create_test_report(args, test_results)

    target.close()
    target_truth.close()

    if not res:
        sys.exit(1)


if __name__ == "__main__":
    main()
