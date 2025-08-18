#!/usr/bin/env python3

# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2020 Damien P. George
#
# run-multitests.py
# Runs a test suite that relies on two micropython instances/devices
# interacting in some way. Typically used to test networking / bluetooth etc.


import sys, os, time, re, select
import argparse
import itertools
import subprocess
import tempfile

run_tests_module = __import__("run-tests")

test_dir = os.path.abspath(os.path.dirname(__file__))

if os.path.abspath(sys.path[0]) == test_dir:
    # remove the micropython/tests dir from path to avoid
    # accidentally importing tests like micropython/const.py
    sys.path.pop(0)

sys.path.insert(0, test_dir + "/../tools")
import pyboard

if os.name == "nt":
    CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3.exe")
    MICROPYTHON = os.path.abspath(
        os.getenv(
            "MICROPY_MICROPYTHON", test_dir + "/../ports/windows/build-standard/micropython.exe"
        )
    )
else:
    CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3")
    MICROPYTHON = os.path.abspath(
        os.getenv("MICROPY_MICROPYTHON", test_dir + "/../ports/unix/build-standard/micropython")
    )

# For diff'ing test output
DIFF = os.getenv("MICROPY_DIFF", "diff -u")

PYTHON_TRUTH = CPYTHON3

INSTANCE_READ_TIMEOUT_S = 10

APPEND_CODE_TEMPLATE = """
import sys
class multitest:
    @staticmethod
    def flush():
        try:
            sys.stdout.flush()
        except AttributeError:
            pass
    @staticmethod
    def skip():
        print("SKIP")
        multitest.flush()
        raise SystemExit
    @staticmethod
    def next():
        print("NEXT")
        multitest.flush()
    @staticmethod
    def broadcast(msg):
        print("BROADCAST", msg)
        multitest.flush()
    @staticmethod
    def wait(msg):
        msg = "BROADCAST " + msg
        while True:
            if sys.stdin.readline().rstrip() == msg:
                return
    @staticmethod
    def globals(**gs):
        for g in gs:
            print("SET {{}} = {{!r}}".format(g, gs[g]))
        multitest.flush()
    @staticmethod
    def get_network_ip():
        try:
            ip = nic.ifconfig()[0]
        except:
            try:
                import network
                if hasattr(network, "WLAN"):
                    ip = network.WLAN().ifconfig()[0]
                else:
                    ip = network.LAN().ifconfig()[0]
            except:
                ip = HOST_IP
        return ip
    @staticmethod
    def expect_reboot(resume, delay_ms=0):
        print("WAIT_FOR_REBOOT", resume, delay_ms)
    @staticmethod
    def output_metric(data):
        print("OUTPUT_METRIC", data)

{}

instance{}()
multitest.flush()
"""

# Some ports generate output we can't control, and that can be safely ignored.
IGNORE_OUTPUT_MATCHES = (
    "libusb: error ",  # unix btstack tries to open devices that it doesn't have access to (libusb prints unconditionally).
    "hci_transport_h2_libusb.c",  # Same issue. We enable LOG_ERROR in btstack.
    "USB Path: ",  # Hardcoded in unix btstack's libusb transport.
    "hci_number_completed_packet",  # Warning from unix btstack.
    "lld_pdu_get_tx_flush_nb HCI packet count mismatch (",  # From ESP-IDF, see https://github.com/espressif/esp-idf/issues/5105
    " ets_task(",  # ESP8266 port debug output
)


def get_host_ip(_ip_cache=[]):
    if not _ip_cache:
        try:
            import socket

            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.connect(("8.8.8.8", 80))
            _ip_cache.append(s.getsockname()[0])
            s.close()
        except:
            _ip_cache.append("127.0.0.1")
    return _ip_cache[0]


def decode(output):
    # Convenience function to convert raw process or serial output to ASCII
    return str(output, "ascii", "backslashreplace")


class PyInstance:
    def __init__(self):
        pass

    def close(self):
        pass

    def prepare_script_from_file(self, filename, prepend, append):
        with open(filename, "rb") as f:
            script = f.read()
        if prepend:
            script = bytes(prepend, "ascii") + b"\n" + script
        if append:
            script += b"\n" + bytes(append, "ascii")
        return script

    def run_file(self, filename, prepend="", append=""):
        return self.run_script(self.prepare_script_from_file(filename, prepend, append))

    def start_file(self, filename, prepend="", append=""):
        return self.start_script(self.prepare_script_from_file(filename, prepend, append))


class PyInstanceSubProcess(PyInstance):
    def __init__(self, argv, env=None):
        self.argv = argv
        self.cwd = None
        self.env = {n: v for n, v in (i.split("=") for i in env)} if env else None
        self.popen = None
        self.finished = True

    def __str__(self):
        return self.argv[0].rsplit("/")[-1]

    def prepare_script_from_file(self, filename, prepend, append):
        # Make tests run in the directory of the test file, and in an isolated environment
        # (i.e. `import io` would otherwise get the `tests/io` directory).
        self.cwd = os.path.dirname(filename)
        remove_cwd_from_sys_path = b"import sys\nsys.path.remove('')\n\n"
        return remove_cwd_from_sys_path + super().prepare_script_from_file(
            filename, prepend, append
        )

    def run_script(self, script):
        output = b""
        err = None
        try:
            p = subprocess.run(
                self.argv,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                input=script,
                cwd=self.cwd,
                env=self.env,
            )
            output = p.stdout
        except subprocess.CalledProcessError as er:
            err = er
        return decode(output.strip()), err

    def start_script(self, script):
        self.popen = subprocess.Popen(
            self.argv + ["-c", script],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=self.cwd,
            env=self.env,
        )
        self.finished = False

    def stop(self):
        if self.popen and self.popen.poll() is None:
            self.popen.terminate()

    def readline(self):
        sel = select.select([self.popen.stdout.raw], [], [], 0.001)
        if not sel[0]:
            self.finished = self.popen.poll() is not None
            return None, None
        out = self.popen.stdout.raw.readline()
        if out == b"":
            self.finished = self.popen.poll() is not None
            return None, None
        else:
            return decode(out.rstrip()), None

    def write(self, data):
        self.popen.stdin.write(data)
        self.popen.stdin.flush()

    def is_finished(self):
        return self.finished

    def wait_finished(self):
        self.popen.wait()
        out = self.popen.stdout.read()
        return decode(out), ""


class PyInstancePyboard(PyInstance):
    def __init__(self, device):
        device = device
        self.device = device
        self.pyb = pyboard.Pyboard(device)
        self.pyb.enter_raw_repl()
        self.finished = True

    def __str__(self):
        return self.device.rsplit("/")[-1]

    def close(self):
        self.pyb.exit_raw_repl()
        self.pyb.close()

    def run_script(self, script):
        output = b""
        err = None
        try:
            self.pyb.enter_raw_repl()
            output = self.pyb.exec_(script)
        except pyboard.PyboardError as er:
            err = er
        return decode(output.strip()), err

    def start_script(self, script):
        self.pyb.enter_raw_repl()
        self.pyb.exec_raw_no_follow(script)
        self.finished = False

    def stop(self):
        self.pyb.serial.write(b"\r\x03")

    def readline(self):
        if self.finished:
            return None, None
        if self.pyb.serial.inWaiting() == 0:
            return None, None
        out = self.pyb.read_until(1, (b"\r\n", b"\x04"))
        if out.endswith(b"\x04"):
            self.finished = True
            out = out[:-1]
            err = decode(self.pyb.read_until(1, b"\x04"))
            err = err[:-1]
            if not out and not err:
                return None, None
        else:
            err = None
        return decode(out.rstrip()), err

    def write(self, data):
        self.pyb.serial.write(data)

    def is_finished(self):
        return self.finished

    def wait_finished(self):
        out, err = self.pyb.follow(10, None)
        return decode(out), decode(err)


def prepare_test_file_list(test_files):
    test_files2 = []
    for test_file in sorted(test_files):
        num_instances = 0
        with open(test_file) as f:
            for line in f:
                m = re.match(r"def instance([0-9]+)\(\):", line)
                if m:
                    num_instances = max(num_instances, int(m.group(1)) + 1)
        test_files2.append((test_file, num_instances))
    return test_files2


def trace_instance_output(instance_idx, line):
    if cmd_args.trace_output:
        t_ms = round((time.time() - trace_t0) * 1000)
        print("{:6} i{} :".format(t_ms, instance_idx), line)
        sys.stdout.flush()


def run_test_on_instances(test_file, num_instances, instances):
    global trace_t0
    trace_t0 = time.time()

    error = False
    skip = False
    injected_globals = ""
    output = [[] for _ in range(num_instances)]
    output_metrics = []

    # If the test calls get_network_ip() then inject HOST_IP so that devices can know
    # the IP address of the host.  Do this lazily to not require a TCP/IP connection
    # on the host if it's not needed.
    with open(test_file, "rb") as f:
        if b"get_network_ip" in f.read():
            injected_globals += "HOST_IP = '" + get_host_ip() + "'\n"

    if cmd_args.trace_output:
        print("TRACE {}:".format("|".join(str(i) for i in instances)))

    # Start all instances running, in order, waiting until they signal they are ready
    for idx in range(num_instances):
        append_code = APPEND_CODE_TEMPLATE.format(injected_globals, idx)
        instance = instances[idx]
        instance.start_file(test_file, append=append_code)
        last_read_time = time.time()
        while True:
            if instance.is_finished():
                break
            out, err = instance.readline()
            if out is None and err is None:
                if time.time() > last_read_time + INSTANCE_READ_TIMEOUT_S:
                    output[idx].append("TIMEOUT")
                    error = True
                    break
                time.sleep(0.1)
                continue
            last_read_time = time.time()
            if out is not None and not any(m in out for m in IGNORE_OUTPUT_MATCHES):
                trace_instance_output(idx, out)
                if out.startswith("SET "):
                    injected_globals += out[4:] + "\n"
                elif out == "SKIP":
                    skip = True
                    break
                elif out == "NEXT":
                    break
                else:
                    output[idx].append(out)
            if err is not None:
                trace_instance_output(idx, err)
                output[idx].append(err)
                error = True

        if error or skip:
            break

    if not error and not skip:
        # Capture output and wait for all instances to finish running
        last_read_time = [time.time() for _ in range(num_instances)]
        while True:
            num_running = 0
            num_output = 0
            for idx in range(num_instances):
                instance = instances[idx]
                if instance.is_finished():
                    continue
                num_running += 1
                out, err = instance.readline()
                if out is None and err is None:
                    if time.time() > last_read_time[idx] + INSTANCE_READ_TIMEOUT_S:
                        output[idx].append("TIMEOUT")
                        error = True
                    continue
                num_output += 1
                last_read_time[idx] = time.time()
                if out is not None and not any(m in out for m in IGNORE_OUTPUT_MATCHES):
                    trace_instance_output(idx, out)
                    if out.startswith("WAIT_FOR_REBOOT"):
                        _, resume, delay_ms = out.split(" ")

                        if wait_for_reboot(instance, delay_ms):
                            # Restart the test code, resuming from requested instance block
                            if not resume.startswith("instance{}".format(idx)):
                                raise SystemExit(
                                    'ERROR: resume function must start with "instance{}"'.format(
                                        idx
                                    )
                                )
                            append_code = APPEND_CODE_TEMPLATE.format(injected_globals, resume[8:])
                            instance.start_file(test_file, append=append_code)
                            last_read_time[idx] = time.time()

                    if out.startswith("BROADCAST "):
                        for instance2 in instances:
                            if instance2 is not instance:
                                instance2.write(bytes(out, "ascii") + b"\r\n")
                    elif out.startswith("OUTPUT_METRIC "):
                        output_metrics.append(out.split(" ", 1)[1])
                    else:
                        output[idx].append(out)
                if err is not None:
                    trace_instance_output(idx, err)
                    output[idx].append(err)
                    error = True

            if not num_output:
                time.sleep(0.1)
            if not num_running or error:
                break

    # Stop all instances
    for idx in range(num_instances):
        instances[idx].stop()

    output_str = ""
    for idx, lines in enumerate(output):
        output_str += "--- instance{} ---\n".format(idx)
        output_str += "\n".join(lines) + "\n"

    return error, skip, output_str, output_metrics


def wait_for_reboot(instance, extra_timeout_ms=0):
    # Monitor device responses for reboot banner, waiting for idle.
    extra_timeout = float(extra_timeout_ms) * 1000
    INITIAL_TIMEOUT = 1 + extra_timeout
    FULL_TIMEOUT = 5 + extra_timeout
    t_start = t_last_activity = time.monotonic()
    while True:
        t = time.monotonic()
        out, err = instance.readline()
        if err is not None:
            print("Reboot: communication error", err)
            return False
        if out:
            t_last_activity = t
            # Check for reboot banner, see py/pyexec.c "reset friendly REPL"
            if re.match(r"^MicroPython v\d+\.\d+\.\d+.* on .*; .* with .*$", out):
                time.sleep(0.1)
                break

        if t_last_activity == t_start:
            if t - t_start > INITIAL_TIMEOUT:
                print("Reboot: missed initial Timeout")
                return False
        else:
            if t - t_start > FULL_TIMEOUT:
                print("Reboot: Timeout")
                return False

    instance.pyb.enter_raw_repl()
    return True


def print_diff(a, b):
    a_fd, a_path = tempfile.mkstemp(text=True)
    b_fd, b_path = tempfile.mkstemp(text=True)
    os.write(a_fd, a.encode())
    os.write(b_fd, b.encode())
    os.close(a_fd)
    os.close(b_fd)
    subprocess.run(DIFF.split(" ") + [a_path, b_path])
    os.unlink(a_path)
    os.unlink(b_path)


def run_tests(test_files, instances_truth, instances_test):
    test_results = []

    for test_file, num_instances in test_files:
        instances_str = "|".join(str(instances_test[i]) for i in range(num_instances))
        print("{} on {}: ".format(test_file, instances_str), end="")
        if cmd_args.show_output or cmd_args.trace_output:
            print()
        sys.stdout.flush()

        # Run test on test instances
        error, skip, output_test, output_metrics = run_test_on_instances(
            test_file, num_instances, instances_test
        )

        if not skip:
            # Check if truth exists in a file, and read it in
            test_file_expected = test_file + ".exp"
            if os.path.isfile(test_file_expected):
                with open(test_file_expected) as f:
                    output_truth = f.read()
            else:
                # Run test on truth instances to get expected output
                _, _, output_truth, _ = run_test_on_instances(
                    test_file, num_instances, instances_truth
                )

        if cmd_args.show_output:
            print("### TEST ###")
            print(output_test, end="")
            if not skip:
                print("### TRUTH ###")
                print(output_truth, end="")

        # Print result of test
        if skip:
            print("skip")
            test_results.append((test_file, "skip", ""))
        elif output_test == output_truth:
            print("pass")
            test_results.append((test_file, "pass", ""))
        else:
            print("FAIL")
            test_results.append((test_file, "fail", ""))
            if not cmd_args.show_output:
                print("### TEST ###")
                print(output_test, end="")
                print("### TRUTH ###")
                print(output_truth, end="")
                print("### DIFF ###")
                print_diff(output_truth, output_test)

        # Print test output metrics, if there are any.
        if output_metrics:
            for metric in output_metrics:
                print(test_file, ": ", metric, sep="")

        if cmd_args.show_output:
            print()

    return test_results


def main():
    global cmd_args

    cmd_parser = argparse.ArgumentParser(
        description="Run network tests for MicroPython",
        epilog=(
            run_tests_module.test_instance_epilog
            + "Each instance arg can optionally have custom env provided, eg. <cmd>,ENV=VAR,ENV=VAR...\n"
        ),
        formatter_class=argparse.RawTextHelpFormatter,
    )
    cmd_parser.add_argument(
        "-s", "--show-output", action="store_true", help="show test output after running"
    )
    cmd_parser.add_argument(
        "-c", "--trace-output", action="store_true", help="trace test output while running"
    )
    cmd_parser.add_argument(
        "-t",
        "--test-instance",
        action="append",
        default=[],
        help="instance(s) to run the tests on",
    )
    cmd_parser.add_argument(
        "-p",
        "--permutations",
        type=int,
        default=1,
        help="repeat the test with this many permutations of the instance order",
    )
    cmd_parser.add_argument(
        "-r",
        "--result-dir",
        default=run_tests_module.base_path("results"),
        help="directory for test results",
    )
    cmd_parser.add_argument("files", nargs="+", help="input test files")
    cmd_args = cmd_parser.parse_args()

    # clear search path to make sure tests use only builtin modules and those in extmod
    os.environ["MICROPYPATH"] = os.pathsep.join((".frozen", "../extmod"))

    test_files = prepare_test_file_list(cmd_args.files)
    max_instances = max(t[1] for t in test_files)

    instances_truth = [PyInstanceSubProcess([PYTHON_TRUTH]) for _ in range(max_instances)]

    instances_test = []
    for i in cmd_args.test_instance:
        # Each instance arg is <cmd>,ENV=VAR,ENV=VAR...
        i = i.split(",")
        cmd = i[0]
        env = i[1:]
        if cmd.startswith("exec:"):
            instances_test.append(PyInstanceSubProcess([cmd[len("exec:") :]], env))
        elif cmd == "unix":
            instances_test.append(PyInstanceSubProcess([MICROPYTHON], env))
        elif cmd == "cpython":
            instances_test.append(PyInstanceSubProcess([CPYTHON3], env))
        elif cmd == "webassembly" or cmd.startswith("execpty:"):
            print("unsupported instance string: {}".format(cmd), file=sys.stderr)
            sys.exit(2)
        else:
            device = run_tests_module.convert_device_shortcut_to_real_device(cmd)
            instances_test.append(PyInstancePyboard(device))

    for _ in range(max_instances - len(instances_test)):
        instances_test.append(PyInstanceSubProcess([MICROPYTHON]))

    os.makedirs(cmd_args.result_dir, exist_ok=True)
    all_pass = True
    try:
        for i, instances_test_permutation in enumerate(itertools.permutations(instances_test)):
            if i >= cmd_args.permutations:
                break

            test_results = run_tests(test_files, instances_truth, instances_test_permutation)
            all_pass &= run_tests_module.create_test_report(cmd_args, test_results)

    finally:
        for i in instances_truth:
            i.close()
        for i in instances_test:
            i.close()

    if not all_pass:
        sys.exit(1)


if __name__ == "__main__":
    main()
