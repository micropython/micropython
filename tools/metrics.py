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

On ESP-IDF ELFs, Berkeley `size`'s `data` column mixes DRAM `.data` with flash
`.rodata` (DROM). For the esp32 port, `sizes` also emits an `ESP32_SECTIONS`
block from `size -A` so `diff` can report `(dram.data)`, `(bss)`, `(iram)` and
`(rodata)` separately, plus `(other.data)` / `(other.bss)` for any remaining
Berkeley delta. Other ports are unchanged.

"""

import collections, os, sys, re, shlex, subprocess, multiprocessing

MAKE_FLAGS = ["-j{}".format(multiprocessing.cpu_count()), "CFLAGS_EXTRA=-DNDEBUG"]

# Section sizes reported for esp32 via `size -A` (ESP-IDF linker layout).
ESP32_SECTION_NAMES = (
    ".dram0.data",
    ".dram0.bss",
    ".iram0.text",
    ".flash.rodata",
    ".flash.text",
)
# Diff annotation labels (flash.text omitted; covered by headline dec).
ESP32_DIFF_SECTIONS = (
    (".dram0.data", "dram.data"),
    (".dram0.bss", "bss"),
    (".iram0.text", "iram"),
    (".flash.rodata", "rodata"),
)
# Sections above that Berkeley `size` accounts for in its data and bss columns.
# Anything left over lands in other.data / other.bss, so growth in sections that
# are not broken out above (eg .iram0.bss, .noinit, .rtc.data) is still reported.
ESP32_BERKELEY_COLUMNS = (
    (1, (".dram0.data", ".flash.rodata"), "other.data"),
    (2, (".dram0.bss",), "other.bss"),
)


class PortData:
    def __init__(self, name, dir, output, make_flags=None, pre_cmd=None):
        self.name = name
        self.dir = dir
        self.output = output
        self.make_flags = make_flags
        self.needs_mpy_cross = dir not in ("bare-arm", "minimal")
        self.pre_cmd = pre_cmd


mpy_cross_output = "mpy-cross/build/mpy-cross"

port_data = {
    "b": PortData("bare-arm", "bare-arm", "build/firmware.elf"),
    "m": PortData("minimal x86", "minimal", "build/firmware.elf"),
    "u": PortData("unix x64", "unix", "build-standard/micropython"),
    "n": PortData("unix nanbox", "unix", "build-nanbox/micropython", "VARIANT=nanbox"),
    "s": PortData("stm32", "stm32", "build-PYBV10/firmware.elf", "BOARD=PYBV10"),
    "c": PortData("cc3200", "cc3200", "build/WIPY/release/application.axf", "BTARGET=application"),
    "8": PortData("esp8266", "esp8266", "build-ESP8266_GENERIC/firmware.elf"),
    "3": PortData(
        "esp32",
        "esp32",
        "build-ESP32_GENERIC/micropython.elf",
        pre_cmd=". esp-idf/export.sh",
    ),
    "x": PortData("mimxrt", "mimxrt", "build-TEENSY40/firmware.elf"),
    "e": PortData("renesas-ra", "renesas-ra", "build-EK_RA6M2/firmware.elf"),
    "r": PortData("nrf", "nrf", "build-PCA10040/firmware.elf"),
    "p": PortData("rp2", "rp2", "build-RPI_PICO_W/firmware.elf", "BOARD=RPI_PICO_W"),
    "d": PortData("samd", "samd", "build-ADAFRUIT_ITSYBITSY_M4_EXPRESS/firmware.elf"),
    "v": PortData("qemu rv32", "qemu", "build-VIRT_RV32/firmware.elf", "BOARD=VIRT_RV32"),
}

for port_letter, port in port_data.items():
    port.pre_cmd = os.environ.get(f"PRE_CMD_{port_letter}", port.pre_cmd)


def quoted(args):
    return " ".join(shlex.quote(word) for word in args)


def syscmd(*args, pre_cmd=None):
    sys.stdout.flush()
    a2 = []
    for a in args:
        if isinstance(a, str):
            a2.append(a)
        elif a:
            a2.extend(a)
    if pre_cmd is not None:
        a2_quoted = quoted(a2)
        a2 = ["bash", "-c", "{} && {}".format(pre_cmd, a2_quoted)]
        print(a2)
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


def parse_esp32_sections_output(size_a_output):
    """Parse `size -A` text; return dict of wanted ESP-IDF section sizes."""
    sizes = {}
    for line in size_a_output.splitlines():
        parts = line.split()
        if len(parts) >= 2 and parts[0] in ESP32_SECTION_NAMES:
            try:
                sizes[parts[0]] = int(parts[1])
            except ValueError:
                pass
    return sizes


def print_esp32_sections(elf_path):
    """Emit ESP32_SECTIONS block for elf_path, or nothing if size -A fails."""
    try:
        out = subprocess.check_output(["size", "-A", elf_path], text=True)
    except (OSError, subprocess.CalledProcessError):
        return
    sizes = parse_esp32_sections_output(out)
    if not sizes:
        return
    print("ESP32_SECTIONS", elf_path)
    for name in ESP32_SECTION_NAMES:
        if name in sizes:
            print(name, sizes[name])


def read_build_log(filename):
    data = collections.OrderedDict()
    lines = []
    found_sizes = False
    with open(filename) as f:
        for line in f:
            line = line.strip()
            if line.startswith("BUILDING ") and "_ref" not in data:
                data["_ref"] = line.removeprefix("BUILDING ")
            if line.strip() == "COMPUTING SIZES":
                found_sizes = True
            elif found_sizes:
                lines.append(line)
    is_size_line = False
    esp32_sections = {}
    current_esp32 = None
    for line in lines:
        if line.startswith("ESP32_SECTIONS "):
            current_esp32 = line.split(None, 1)[1]
            esp32_sections[current_esp32] = {}
            is_size_line = False
            continue
        if current_esp32 is not None:
            parts = line.split()
            if len(parts) == 2 and parts[0].startswith("."):
                try:
                    esp32_sections[current_esp32][parts[0]] = int(parts[1])
                    continue
                except ValueError:
                    pass
            current_esp32 = None
        if is_size_line:
            fields = line.split()
            data[fields[-1]] = [int(f) for f in fields[:-2]]
            is_size_line = False
        else:
            is_size_line = line.startswith("text\t ")
    if esp32_sections:
        data["_esp32_sections"] = esp32_sections
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

    ref1 = data1.pop("_ref", "(unknown ref)")
    ref2 = data2.pop("_ref", "(unknown ref)")
    sections1 = data1.pop("_esp32_sections", {})
    sections2 = data2.pop("_esp32_sections", {})
    print(f"Reference:  {ref1}")
    print(f"Comparison: {ref2}")
    max_delta = None
    for key, value1 in data1.items():
        value2 = data2[key]
        if key == mpy_cross_output:
            name = "mpy-cross"
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
            sec1 = sections1.get(key) if name == "esp32" else None
            sec2 = sections2.get(key) if name == "esp32" else None
            if sec1 is not None and sec2 is not None:
                deltas = {
                    sec_name: sec2.get(sec_name, 0) - sec1.get(sec_name, 0)
                    for sec_name in ESP32_SECTION_NAMES
                }
                for sec_name, label in ESP32_DIFF_SECTIONS:
                    if deltas[sec_name] != 0:
                        warn += " %+d(%s)" % (deltas[sec_name], label)
                for column, sec_names, label in ESP32_BERKELEY_COLUMNS:
                    other = data[column] - sum(deltas[sec_name] for sec_name in sec_names)
                    if other != 0:
                        warn += " %+d(%s)" % (other, label)
            else:
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

    if any(port.needs_mpy_cross for port in ports):
        syscmd("make", "-C", "mpy-cross", "clean")

    for port in ports:
        syscmd(
            "make",
            "-C",
            "ports/{}".format(port.dir),
            port.make_flags,
            "clean",
            pre_cmd=port.pre_cmd,
        )


def do_build(args):
    """Build ports and print firmware sizes."""

    ports = parse_port_list(args)

    if any(port.needs_mpy_cross for port in ports):
        print("BUILDING MPY-CROSS")
        syscmd("make", "-C", "mpy-cross", MAKE_FLAGS)

    print("BUILDING PORTS")
    for port in ports:
        syscmd(
            "make",
            "-C",
            "ports/{}".format(port.dir),
            MAKE_FLAGS,
            port.make_flags,
            pre_cmd=port.pre_cmd,
        )

    do_sizes(args)


def do_sizes(args):
    """Compute and print sizes of firmware."""

    ports = parse_port_list(args)

    print("COMPUTING SIZES")

    if any(port.needs_mpy_cross for port in ports):
        syscmd("size", mpy_cross_output)

    for port in ports:
        elf = "ports/{}/{}".format(port.dir, port.output)
        syscmd("size", elf)
        if port.name == "esp32":
            print_esp32_sections(elf)


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
