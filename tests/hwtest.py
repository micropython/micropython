#!/usr/bin/env python3
# MIT license; Copyright (c) 2024 Damien P. George

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


class Target:
    def __init__(self, device, serial_number):
        self.device = device
        self.device_suffix = device.split("/")[-1]
        self.serial_number = serial_number
        self.port = None
        self.build = None
        self.machine = None
        self.has_wlan_connected = False

    def info(self):
        features = ""
        if self.can_import_mpy:
            features += " import-mpy"
        if self.has_wlan:
            features += " WLAN"
        if self.has_ble:
            features += " BLE"
        return f"{self.device} {self.port} {self.build} {self.arch}{features} {self.version}"


def map_device_name(d):
    if d[0] == "a" and d[1:].isdigit():
        return "/dev/ttyACM" + d[1:]
    if d[0] == "u" and d[1:].isdigit():
        return "/dev/ttyUSB" + d[1:]
    return d


def list_ports(args):
    ports = []
    for p in sorted(serial.tools.list_ports.comports()):
        if p.device.startswith("/dev/ttyS"):
            # Skip invalid ttySx ports.
            continue
        if not args or p.device in args:
            ports.append(Target(p.device, p.serial_number))
    return ports


NATMOD_ARCHS = ("x86", "x64", "armv6", "armv6m", "armv7m", "armv7em", "armv7emsp", "armv7emdp", "xtensa", "xtensawin", "rv32imc")
NATMOD_LIBS = ("btree", "deflate", "framebuf", "heapq", "random", "re")


def build_natmods():
    for arch in NATMOD_ARCHS:
        for lib in NATMOD_LIBS:
            subprocess.run(["make", "-C", f"../examples/natmod/{lib}", "-j8", "-B", f"ARCH={arch}"])


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


def run_multitests_on_one_target(targets, tests):
    for target in targets:
        do_test(["./run-multitests.py", "-i", f"pyb:{target.device}", "-p2"] + tests)


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


def main():
    args = sys.argv[1:]

    if len(args) > 0 and args[0] == "build-natmod":
        build_natmods()
        return

    selected_tests = "vphmnwb"
    if len(args) > 0:
        # Select certain tests.
        if (a := args.pop(0)) != "all":
            selected_tests = a

    with open("feature_check/target_info.py", "rb") as f:
        target_info_check = f.read()

    print("Selected tests:", selected_tests)

    selected_devices = [map_device_name(d) for d in args]
    targets = list_ports(selected_devices)

    select_vcprate = "v" in selected_tests
    select_python = "p" in selected_tests
    select_hardware = "h" in selected_tests
    select_via_mpy = "m" in selected_tests
    select_native = "n" in selected_tests
    select_wlan = "w" in selected_tests
    select_ble = "b" in selected_tests

    targets_ble = []
    for target in targets:
        t = SerialTransport(target.device)
        t.enter_raw_repl()
        t.exec("import sys")
        sys_info = t.eval(
            "(sys.platform, getattr(sys.implementation, '_build', 'UNKNOWN'), sys.implementation._machine, sys.version)"
        )
        target.port, target.build, target.machine, target.version = sys_info
        target.can_import_mpy = t.eval("hasattr(sys.implementation, '_mpy')")
        _, target.arch = str(t.exec(target_info_check), "ascii").strip().split()
        if target.arch == "None":
            target.arch = None
        target.has_vfs = try_import(t, "vfs")
        target.has_filesystem = target.has_vfs and t.eval("len(vfs.mount())")
        target.has_unittest = try_import(t, "unittest")
        target.has_wlan = try_import(t, "network") and t.eval("hasattr(network, 'WLAN')")
        target.has_ble = try_import(t, "bluetooth") and t.eval("hasattr(bluetooth, 'BLE')")
        t.close()

    for target in targets:
        print(target.info())

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
                    port_specific.extend(("-d", "basics", "extmod", "float"))
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
                t = SerialTransport(target.device)
                t.enter_raw_repl()
                rtc_set(target, t)
                if target.has_filesystem:
                    for file in glob.glob("net_inet/*.der") + glob.glob("multi_net/*.der"):
                        put_file(t, *file.rsplit("/", 1))
                target.has_wlan_connected = wlan_connect(target, t)
                t.close()
                if target.has_wlan_connected:
                    do_test(run_tests_cmd + ["-d", "net_hosted", "net_inet"])

        targets_wlan = [t for t in targets if t.has_wlan_connected]
        if select_wlan and targets_wlan:
            print("=" * 64)
            run_multitests_on_one_target(targets_wlan, tests_multi_net)
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
