#!/usr/bin/env python3
# MIT license; Copyright (c) 2024 Damien P. George

import argparse
import sys
import glob
import os
import subprocess
import serial.tools.list_ports
import datetime
import time
import vcprate

sys.path.append("../tools/mpremote")
from mpremote.transport_serial import SerialTransport
import mpremote.commands
import mpremote.mip

WLAN_SSID = os.getenv("WLAN_SSID")
WLAN_PASS = os.getenv("WLAN_PASS")

if not WLAN_SSID or not WLAN_PASS:
    print("WLAN_SSID and/or WLAN_PASS not provided")

with open("feature_check/target_info.py", "rb") as f:
    TARGET_INFO_CHECK = f.read()


class Target:
    def __init__(self, device, serial_number):
        self.device = device
        self.device_suffix = device.split("/")[-1]
        self.serial_number = serial_number
        self.port = None
        self.build = None
        self.machine = None
        self.can_import_mpy = None
        self.has_vfs = None
        self.has_filesystem = None
        self.has_unittest = None
        self.has_wlan = None
        self.has_wlan_connected = False
        self.has_ble = None

    def info(self):
        features = ""
        if self.can_import_mpy:
            features += " import-mpy"
        if self.has_wlan:
            features += " WLAN"
        if self.has_ble:
            features += " BLE"
        return f"{self.device} {self.port} {self.build} {self.arch}{features} {self.version}"

    def detect_features(self):
        t = SerialTransport(self.device)
        t.enter_raw_repl()
        t.exec("import sys")
        sys_info = t.eval(
            "(sys.platform, getattr(sys.implementation, '_build', 'UNKNOWN'), sys.implementation._machine, sys.version)"
        )
        self.port, self.build, self.machine, self.version = sys_info
        self.can_import_mpy = t.eval("hasattr(sys.implementation, '_mpy')")
        _, self.arch, *_ = str(t.exec(TARGET_INFO_CHECK), "ascii").strip().split()
        if self.arch == "None":
            self.arch = None
        self.has_vfs = try_import(t, "vfs")
        self.has_filesystem = self.has_vfs and t.eval("len(vfs.mount())")
        self.has_unittest = try_import(t, "unittest")
        self.has_wlan = try_import(t, "network") and t.eval("hasattr(network, 'WLAN')")
        self.has_ble = try_import(t, "bluetooth") and t.eval("hasattr(bluetooth, 'BLE')")
        t.close()

    def setup_wlan(self):
        t = SerialTransport(self.device)
        t.enter_raw_repl()
        rtc_set(self, t)
        if self.has_filesystem:
            for file in glob.glob("net_inet/*.der") + glob.glob("multi_net/*.der"):
                put_file(t, *file.rsplit("/", 1))
        self.has_wlan_connected = wlan_connect(self, t)
        t.close()


def map_device_name(d):
    if d[0] == "a" and d[1:].isdigit():
        return "/dev/ttyACM" + d[1:]
    if d[0] == "u" and d[1:].isdigit():
        return "/dev/ttyUSB" + d[1:]
    return d


def detect_octoprobe(device):
    t = SerialTransport(device)
    t.enter_raw_repl()
    t.exec("import os")
    is_octoprobe_infra = "OCTOPROBE" in t.eval("os.listdir()")
    t.close()
    return is_octoprobe_infra


def list_targets(args, exclude_devices):
    targets = []
    valid_serial_ports = sorted(serial.tools.list_ports.comports())
    if args:
        for device in args:
            for p in valid_serial_ports:
                if device == p.device:
                    if detect_octoprobe(device):
                        print(f"{device} Octoprobe infrastructure device")
                    targets.append(Target(device, p.serial_number))
                    break
            else:
                print(f"{device} not found")
                sys.exit(1)
    else:
        for p in valid_serial_ports:
            if p.device.startswith("/dev/ttyS"):
                # Skip invalid ttySx ports.
                continue
            if p.device in exclude_devices:
                continue
            if detect_octoprobe(p.device):
                # If no devices specified, skip Octoprobe infrastructure.
                print(f"{p.device} Octoprobe infrastructure device, skipping")
                continue
            targets.append(Target(p.device, p.serial_number))
    return targets


NATMOD_ARCHS = (
    "x86",
    "x64",
    "armv6m",
    "armv7m",
    "armv7emsp",
    "armv7emdp",
    "xtensa",
    "xtensawin",
    "rv32imc",
)
NATMOD_LIBS = ("btree", "deflate", "framebuf", "heapq", "random", "re")


def build_natmods(archs):
    if not archs:
        archs = NATMOD_ARCHS
    for arch in archs:
        for lib in NATMOD_LIBS:
            subprocess.run(
                ["make", "-C", f"../examples/natmod/{lib}", "-j8", "-B", f"ARCH={arch}"]
            )


def try_import(target, module):
    return (
        target.exec(f"try:\n import {module}\n print(True)\nexcept:\n print(False)").strip()
        == b"True"
    )


def mip_install(target, target_serial, package):
    class args:
        command = ("install",)
        packages = (package,)
        index = None
        target = None
        mpy = True

    class state:
        transport = target_serial
        did_action = lambda: None
        ensure_raw_repl = lambda: target_serial.enter_raw_repl()

    mpremote.mip.do_mip(state, args)


def rtc_set(target, serial):
    now = datetime.datetime.now()
    time.sleep(1 - now.microsecond * 1e-6)
    now = datetime.datetime.now()
    print(target.device, "set time to", now)
    timetuple = "({}, {}, {}, {}, {}, {}, {}, {})".format(
        now.year,
        now.month,
        now.day,
        now.weekday(),
        now.hour,
        now.minute,
        now.second,
        now.microsecond,
    )
    serial.exec("import machine;machine.RTC().datetime({})".format(timetuple))


def put_file(target_serial, path, file):
    class state:
        transport = target_serial

    mpremote.commands.do_filesystem_cp(state, os.path.join(path, file), ":" + file, False, True)


def wlan_connect(target, target_serial):
    print(f"{target.device} connecting to {WLAN_SSID}... ", end="")
    sys.stdout.flush()
    out = target_serial.exec(f"""
import sys, machine, network, time
wl = network.WLAN()
wl.active(1)
if not wl.isconnected():
    wl.connect('{WLAN_SSID}', '{WLAN_PASS}')
    t0 = time.ticks_ms()
    print('connect')
    while not wl.isconnected() and time.ticks_diff(time.ticks_ms(), t0) < 10_000:
        machine.idle()
""")
    isconnected, ifconfig = target_serial.eval("(wl.isconnected(), wl.ifconfig())")
    if isconnected:
        print(ifconfig[0])
    else:
        print("FAIL")
    return isconnected


def do_test(cmd):
    print("-" * 32)
    print("RUN", " ".join(cmd))
    try:
        result = subprocess.run(cmd)
        if result.returncode == 0:
            print("PASS")
        else:
            print(f"ERROR returncode={result.returncode}")
        time.sleep(2)
    except KeyboardInterrupt:
        print("INTERRUPT")
        time.sleep(2)


def run_multitests_p2(instances, tests):
    cmd = ["./run-multitests.py", "-p2"]
    for instance in instances:
        cmd.extend(["-i", f"pyb:{instance}"])
    do_test(cmd + tests)


def run_multitests_on_two_targets(targets, tests):
    if len(targets) == 1:
        return
    for i in range(len(targets)):
        target0 = targets[i]
        target1 = targets[(i + 1) % len(targets)]
        do_test(
            ["./run-multitests.py", "-i", f"pyb:{target0.device}", "-i", f"pyb:{target1.device}"]
            + tests
        )


ALL_TEST_CODES = "vphmnwb"


def main():
    cmd_parser = argparse.ArgumentParser(description="Run all tests on hardware targets")
    cmd_parser.add_argument(
        "--build-natmods", action="store_true", help="build all/selected natmods"
    )
    cmd_parser.add_argument("-s", "--tests", default=ALL_TEST_CODES, help="tests to run")
    cmd_parser.add_argument(
        "-r", "--reference", default=None, help="reference device for WLAN and BLE tests"
    )
    cmd_parser.add_argument("test_instances", nargs="*", help="target devices to test")
    args = cmd_parser.parse_args()

    if args.build_natmods:
        build_natmods(args.test_instances)
        return

    selected_tests = args.tests
    for test_code in selected_tests:
        if test_code not in ALL_TEST_CODES:
            print(f"{test_code} is not a valid test code")
            sys.exit(1)

    ref_target = None
    if args.reference:
        ref_device = map_device_name(args.reference)
        ref_target = Target(ref_device, "")
        ref_target.detect_features()

    print("Selected tests:", selected_tests)

    selected_devices = [map_device_name(d) for d in args.test_instances]
    targets = list_targets(selected_devices, [ref_target.device] if ref_target else [])

    select_vcprate = "v" in selected_tests
    select_python = "p" in selected_tests
    select_hardware = "h" in selected_tests
    select_via_mpy = "m" in selected_tests
    select_native = "n" in selected_tests
    select_wlan = "w" in selected_tests
    select_ble = "b" in selected_tests

    for target in targets:
        target.detect_features()

    for target in targets:
        print(target.info())

    if ref_target:
        print("=" * 64)
        print("REFERENCE")
        print(ref_target.info())
        if select_wlan and ref_target.has_wlan:
            ref_target.setup_wlan()

    print("=" * 64)
    print("=" * 64)

    tests_natmod = [
        file for file in glob.iglob("extmod/*.py") if file.split("/")[1].startswith(NATMOD_LIBS)
    ]
    tests_multi_net = [file for file in glob.iglob("multi_net/*.py")]
    tests_multi_wlan = [file for file in glob.iglob("multi_wlan/*.py")]
    tests_multi_bluetooth = [
        file for file in glob.iglob("multi_bluetooth/*.py") if "/ble_" in file
    ]

    try:
        for target in targets:
            print("=" * 64)
            print(target.info())
            print(target.machine, "--", target.version)

            # Install unittest if needed.
            if target.has_filesystem and not target.has_unittest:
                t = SerialTransport(target.device)
                mip_install(target, t, "unittest")
                t.close()

            run_tests_cmd = [
                "./run-tests.py",
                "--test-instance",
                target.device,
                "--result-dir",
                f"results_{target.device_suffix}",
            ]
            run_tests_native_cmd = run_tests_cmd + ["--via-mpy", "--emit", "native"]

            do_test(run_tests_cmd + ["--clean-failures"])

            if select_vcprate:
                try:
                    vcprate.do_test(target.device)
                except KeyboardInterrupt:
                    print("INTERRUPT")
                    time.sleep(2)

            if select_python:
                do_test(run_tests_cmd)

            if select_via_mpy and target.can_import_mpy:
                port_specific = []
                if target.port == "esp8266":
                    # extreme_exc has a stack overflow and crashes the device
                    port_specific.extend(("--exclude", "extreme_exc"))
                do_test(run_tests_cmd + ["--via-mpy"] + port_specific)

            if select_native and target.arch is not None:
                do_test(run_tests_native_cmd)
                do_test(["./run-natmodtests.py", "-p", "-d", target.device] + tests_natmod)

            if select_hardware:
                run_cmds = [run_tests_cmd]
                if select_native and target.arch is not None:
                    run_cmds.append(run_tests_native_cmd)
                for run_cmd in run_cmds:
                    do_test(run_cmd + ["-d", "extmod_hardware"])
                    if target.port == "pyboard":
                        do_test(run_cmd + ["-d", "ports/stm32_hardware"])

            if select_wlan and target.has_wlan:
                target.setup_wlan()
                if target.has_wlan_connected:
                    do_test(run_tests_cmd + ["-d", "net_hosted", "net_inet"])
                    run_multitests_p2([target.device], tests_multi_net)
                    if ref_target and ref_target.has_wlan_connected:
                        run_multitests_p2([target.device, ref_target.device], tests_multi_net)
                        run_multitests_p2([target.device, ref_target.device], tests_multi_wlan)

            if select_ble and target.has_ble:
                if ref_target and ref_target.has_ble:
                    run_multitests_p2([target.device, ref_target.device], tests_multi_bluetooth)

        targets_wlan = [t for t in targets if t.has_wlan_connected]
        if select_wlan and targets_wlan:
            print("=" * 64)
            run_multitests_on_two_targets(targets_wlan, tests_multi_net)
            run_multitests_on_two_targets(targets_wlan, tests_multi_wlan)

        targets_ble = [t for t in targets if t.has_ble]
        if select_ble and targets_ble:
            print("=" * 64)
            run_multitests_on_two_targets(targets_ble, tests_multi_bluetooth)

    except KeyboardInterrupt:
        print("INTERRUPT")


if __name__ == "__main__":
    main()
