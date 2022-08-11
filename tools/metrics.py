#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2020 Damien P. George
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

"""
This script is used to compute metrics, like code size, of the various ports.

Typical usage is:

    $ ./tools/metrics.py build | tee size0
    <wait for build to complete>
    $ git switch new-feature-branch
    $ ./tools/metrics.py build | tee size1
    <wait for build to complete>
    $ ./tools/metrics.py diff size0 size1

Other commands:

    $ ./tools/metrics.py sizes # print all firmware sizes
    $ ./tools/metrics.py clean # clean all ports

"""

import collections, sys, re, subprocess

MAKE_FLAGS = ["-j3", "CFLAGS_EXTRA=-DNDEBUG"]


class PortData:
    def __init__(self, name, dir, output, make_flags=None):
        self.name = name
        self.dir = dir
        self.output = output
        self.make_flags = make_flags
        self.needs_mpy_cross = dir not in ("bare-arm", "minimal")


port_data = {
    "b": PortData("bare-arm", "bare-arm", "build/firmware.elf"),
    "m": PortData("minimal x86", "minimal", "build/firmware.elf"),
    "u": PortData("unix x64", "unix", "build-standard/micropython"),
    "n": PortData("unix nanbox", "unix", "build-nanbox/micropython", "VARIANT=nanbox"),
    "s": PortData("stm32", "stm32", "build-PYBV10/firmware.elf", "BOARD=PYBV10"),
    "c": PortData("cc3200", "cc3200", "build/WIPY/release/application.axf", "BTARGET=application"),
    "8": PortData("esp8266", "esp8266", "build-GENERIC/firmware.elf"),
    "3": PortData("esp32", "esp32", "build-GENERIC/micropython.elf"),
    "x": PortData("mimxrt", "mimxrt", "build-TEENSY40/firmware.elf"),
    "e": PortData("renesas-ra", "renesas-ra", "build-RA6M2_EK/firmware.elf"),
    "r": PortData("nrf", "nrf", "build-pca10040/firmware.elf"),
    "p": PortData("rp2", "rp2", "build-PICO/firmware.elf"),
    "d": PortData("samd", "samd", "build-ADAFRUIT_ITSYBITSY_M4_EXPRESS/firmware.elf"),
}


def syscmd(*args):
    sys.stdout.flush()
    a2 = []
    for a in args:
        if isinstance(a, str):
            a2.append(a)
        elif a:
            a2.extend(a)
    subprocess.check_call(a2)


def parse_port_list(args):
    if not args:
        return list(port_data.values())
    else:
        ports = []
        for arg in args:
            for port_char in arg:
                try:
                    ports.append(port_data[port_char])
                except KeyError:
                    print("unknown port:", port_char)
                    sys.exit(1)
        return ports


def read_build_log(filename):
    data = collections.OrderedDict()
    lines = []
    found_sizes = False
    with open(filename) as f:
        for line in f:
            line = line.strip()
            if line.strip() == "COMPUTING SIZES":
                found_sizes = True
            elif found_sizes:
                lines.append(line)
    is_size_line = False
    for line in lines:
        if is_size_line:
            fields = line.split()
            data[fields[-1]] = [int(f) for f in fields[:-2]]
            is_size_line = False
        else:
            is_size_line = line.startswith("text\t ")
    return data


def do_diff(args):
    """Compute the difference between firmware sizes."""

    # Parse arguments.
    error_threshold = None
    if len(args) >= 2 and args[0] == "--error-threshold":
        args.pop(0)
        error_threshold = int(args.pop(0))

    if len(args) != 2:
        print("usage: %s diff [--error-threshold <x>] <out1> <out2>" % sys.argv[0])
        sys.exit(1)

    data1 = read_build_log(args[0])
    data2 = read_build_log(args[1])

    max_delta = None
    for key, value1 in data1.items():
        value2 = data2[key]
        for port in port_data.values():
            if key == "ports/{}/{}".format(port.dir, port.output):
                name = port.name
                break
        data = [v2 - v1 for v1, v2 in zip(value1, value2)]
        warn = ""
        board = re.search(r"/build-([A-Za-z0-9_]+)/", key)
        if board:
            board = board.group(1)
        else:
            board = ""
        if name == "cc3200":
            delta = data[0]
            percent = 100 * delta / value1[0]
            if data[1] != 0:
                warn += " %+u(data)" % data[1]
        else:
            delta = data[3]
            percent = 100 * delta / value1[3]
            if data[1] != 0:
                warn += " %+u(data)" % data[1]
            if data[2] != 0:
                warn += " %+u(bss)" % data[2]
        if warn:
            warn = "[incl%s]" % warn
        print("%11s: %+5u %+.3f%% %s%s" % (name, delta, percent, board, warn))
        max_delta = delta if max_delta is None else max(max_delta, delta)

    if error_threshold is not None and max_delta is not None:
        if max_delta > error_threshold:
            sys.exit(1)


def do_clean(args):
    """Clean ports."""

    ports = parse_port_list(args)

    print("CLEANING")
    for port in ports:
        syscmd("make", "-C", "ports/{}".format(port.dir), port.make_flags, "clean")


def do_build(args):
    """Build ports and print firmware sizes."""

    ports = parse_port_list(args)

    if any(port.needs_mpy_cross for port in ports):
        print("BUILDING MPY-CROSS")
        syscmd("make", "-C", "mpy-cross", MAKE_FLAGS)

    print("BUILDING PORTS")
    for port in ports:
        syscmd("make", "-C", "ports/{}".format(port.dir), MAKE_FLAGS, port.make_flags)

    do_sizes(args)


def do_sizes(args):
    """Compute and print sizes of firmware."""

    ports = parse_port_list(args)

    print("COMPUTING SIZES")
    for port in ports:
        syscmd("size", "ports/{}/{}".format(port.dir, port.output))


def main():
    # Get command to execute
    if len(sys.argv) == 1:
        print("Available commands:")
        for cmd in globals():
            if cmd.startswith("do_"):
                print("   {:9} {}".format(cmd[3:], globals()[cmd].__doc__))
        sys.exit(1)
    cmd = sys.argv.pop(1)

    # Dispatch to desired command
    try:
        cmd = globals()["do_{}".format(cmd)]
    except KeyError:
        print("{}: unknown command '{}'".format(sys.argv[0], cmd))
        sys.exit(1)
    cmd(sys.argv[1:])


if __name__ == "__main__":
    main()
