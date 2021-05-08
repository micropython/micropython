#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2014-2016 Damien P. George
# SPDX-FileCopyrightText: Copyright (c) 2017 Paul Sokolovsky
# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import os
import re
import serial
import sys
import time

import serial.tools.list_ports
import sh
import shutil


class CPboardError(BaseException):
    pass


# supervisor/messages/default.h:
MSG_NEWLINE = b"\r\n"
MSG_SAFE_MODE_CRASH = b"Looks like our core CircuitPython code crashed hard. Whoops!"
MSG_SAFE_MODE_BROWN_OUT_LINE_1 = (
    b"The microcontroller's power dipped. Please make sure your power supply provides"
)
MSG_SAFE_MODE_BROWN_OUT_LINE_2 = (
    b"enough power for the whole circuit and press reset (after ejecting CIRCUITPY)."
)
MSG_WAIT_BEFORE_REPL = b"Press any key to enter the REPL. Use CTRL-D to reload."


class REPL:
    CHAR_CTRL_A = b"\x01"
    CHAR_CTRL_B = b"\x02"
    CHAR_CTRL_C = b"\x03"
    CHAR_CTRL_D = b"\x04"

    def __init__(self, board):
        self.board = board
        self.write_chunk_size = 32
        self.safe_mode = False
        self.session = b""

    def __enter__(self):
        self.reset()
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        pass

    @property
    def serial(self):
        return self.board.serial

    def read(self):
        if self.serial.in_waiting:
            data = self.serial.read(self.serial.in_waiting)
        else:
            data = b""
        self.session += data
        return data

    def read_until(self, ending, timeout=10):
        data = b""
        timeout_count = 0
        while True:
            if data.endswith(ending):
                break
            elif self.serial.in_waiting > 0:
                new_data = self.serial.read(1)
                data += new_data
                self.session += new_data
                timeout_count = 0
            else:
                timeout_count += 1
                if timeout is not None and timeout_count >= 100 * timeout:
                    raise TimeoutError(110, "timeout waiting for", ending)
                time.sleep(0.01)
        return data

    def write(self, data, chunk_size=None):
        if chunk_size is None:
            chunk_size = self.write_chunk_size
        if not isinstance(data, bytes):
            data = bytes(data, encoding="utf8")

        for i in range(0, len(data), chunk_size):
            chunk = data[i : min(i + chunk_size, len(data))]
            self.session += chunk
            self.serial.write(chunk)
            time.sleep(0.01)

    def reset(self):
        # Use read() since serial.reset_input_buffer() fails with termios.error now and then
        self.read()
        self.session = b""
        self.write(b"\r" + REPL.CHAR_CTRL_C + REPL.CHAR_CTRL_C)  # interrupt any running program
        self.write(b"\r" + REPL.CHAR_CTRL_B)  # enter or reset friendly repl
        data = self.read_until(b">>> ")

    def execute(self, code, timeout=10, wait_for_response=True):
        self.read()  # Throw away

        self.write(REPL.CHAR_CTRL_A)
        self.read_until(b"\r\n>")

        self.write(code)

        self.write(REPL.CHAR_CTRL_D)
        if not wait_for_response:
            return b"", b""
        self.read_until(b"OK")

        output = self.read_until(b"\x04", timeout=timeout)
        output = output[:-1]

        error = self.read_until(b"\x04")
        error = error[:-1]

        return output, error

    def run(self):
        if self.safe_mode:
            raise RuntimeError("Can't run in safe mode")

        self.reset()

        self.write(REPL.CHAR_CTRL_D)
        data = self.read_until(b" output:\r\n")
        if b"Running in safe mode" in data:
            self.safe_mode = True
            raise RuntimeError("Can't run in safe mode")

        # TODO: MSG_SAFE_MODE_CRASH
        # TODO: BROWNOUT

        marker = MSG_NEWLINE + MSG_WAIT_BEFORE_REPL + MSG_NEWLINE
        data = self.read_until(marker)
        data = data.split(marker)[0]

        # Haven't found out why we have to strip off this...
        if data.endswith(b"\r\n\r\n"):
            data = data[:-4]
        return data


class Disk:
    def __init__(self, dev):
        self.dev = os.path.realpath(dev)
        self.mountpoint = None
        with open("/etc/mtab", "r") as f:
            mtab = f.read()
        mount = [mount.split(" ") for mount in mtab.splitlines() if mount.startswith(self.dev)]
        if mount:
            self._path = mount[0][1]
        else:
            name = os.path.basename(dev)
            sh.pmount("-tvfat", dev, name, _timeout=10)
            self.mountpoint = "/media/" + name
            self._path = self.mountpoint

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        try:
            self.close()
        except:
            pass

    @property
    def path(self):
        return self._path

    def close(self):
        if not self.mountpoint:
            return
        mountpoint = self.mountpoint
        self.mountpoint = None

        start_time = time.monotonic()
        unmounted = False
        while not unmounted and start_time - time.monotonic() < 30:
            try:
                sh.pumount(mountpoint)
                unmounted = True
            except sh.ErrorReturnCode_5:
                time.sleep(0.1)

    def sync(self):
        disk_device = os.path.basename(self.dev)[:-1]
        os.sync()
        # Monitor the block device so we know when the sync request is actually finished.
        with open("/sys/block/" + disk_device + "/stat", "r") as f:
            disk_inflight = 1
            last_wait_time = 0
            wait_time = 1
            while disk_inflight > 0 or wait_time > last_wait_time:
                f.seek(0)
                stats = f.read()
                block_stats = stats.split()
                disk_inflight = int(block_stats[8])
                last_wait_time = wait_time
                wait_time = int(block_stats[9])

    def copy(self, src, dst=None, sync=True):
        if dst is None:
            dst = os.path.basename(src)
        shutil.copy(src, os.path.join(self.path, dst))
        if sync:
            self.sync()


class Firmware:
    def __init__(self, board):
        self.board = board

    @property
    def disk(self):
        disks = self.board.get_disks()
        if len(disks) != 1:
            raise RuntimeError("Boot disk not found for: " + self.board.device)
        return Disk(disks[0])

    @property
    def info(self):
        with self.disk as disk:
            fname = os.path.join(disk.path, "INFO_UF2.TXT")
            with open(fname, "r") as f:
                info = f.read()
        lines = info.splitlines()
        res = {}
        res["header"] = lines[0]
        for line in lines[1:]:
            k, _, v = line.partition(":")
            res[k.replace(":", "")] = v.strip()
        return res

    def upload(self, fw):
        with open(fw, "rb") as f:
            header = f.read(32)
        if header[0:4] != b"UF2\n":
            raise ValueError("Only UF2 files are supported")
        self.board.close()
        with self.disk as disk:
            disk.copy(fw, sync=False)


class CPboard:
    @classmethod
    def from_try_all(cls, name, **kwargs):
        try:
            return CPboard.from_build_name(name, **kwargs)
        except ValueError:
            pass

        vendor, _, product = name.partition(":")
        if vendor and product:
            return CPboard.from_usb(**kwargs, idVendor=int(vendor, 16), idProduct=int(product, 16))

        return CPboard(name, **kwargs)

    @classmethod
    def from_build_name(cls, name, **kwargs):
        boards = {
            #'arduino_zero'
            "circuitplayground_express": (0x239A, 0x8019),
            #'feather_m0_adalogger' : (0x239a, ),
            #'feather_m0_basic' : (0x239a, ),
            "feather_m0_express": (0x239A, 0x8023),
            #'feather_m0_rfm69' : (0x239a, ),
            #'feather_m0_rfm9x' : (0x239a, ),
            #'feather_m0_supersized' : (0x239a, ),
            #'feather_m4_express' : (0x239a, ),
            #'gemma_m0' : (0x239a, ),
            #'itsybitsy_m0_express' : (0x239a, ),
            "itsybitsy_m4_express": (0x239A, 0x802C),
            "metro_m0_express": (0x239A, 0x8014),
            "metro_m4_express": (0x239A, 0x8021),
            #'metro_m4_express_revb' : (0x239a, ),
            #'pirkey_m0' : (0x239a, ),
            #'trinket_m0' : (0x239a, ),
            #'trinket_m0_haxpress' : (0x239a, ),
            #'ugame10'
        }

        try:
            vendor, product = boards[name]
        except KeyError:
            raise ValueError("Unknown build name: " + name)

        return CPboard.from_usb(**kwargs, idVendor=vendor, idProduct=product)

    @classmethod
    def from_build_name_bootloader(cls, name, **kwargs):
        boards = {
            #'arduino_zero'
            #'circuitplayground_express' : (0x239a, ),
            #'feather_m0_adalogger' : (0x239a, ),
            #'feather_m0_basic' : (0x239a, ),
            "feather_m0_express": (0x239A, 0x001B),
            #'feather_m0_rfm69' : (0x239a, ),
            #'feather_m0_rfm9x' : (0x239a, ),
            #'feather_m0_supersized' : (0x239a, ),
            #'feather_m4_express' : (0x239a, ),
            #'gemma_m0' : (0x239a, ),
            #'itsybitsy_m0_express' : (0x239a, ),
            "itsybitsy_m4_express": (0x239A, 0x002B),
            #'metro_m0_express' : (0x239a, 0x8014),
            "metro_m4_express": (0x239A, 0x0021),
            #'metro_m4_express_revb' : (0x239a, ),
            #'pirkey_m0' : (0x239a, ),
            #'trinket_m0' : (0x239a, ),
            #'trinket_m0_haxpress' : (0x239a, ),
            #'ugame10'
        }

        try:
            vendor, product = boards[name]
        except KeyError:
            raise ValueError("Unknown build name: " + name)

        board = CPboard.from_usb(**kwargs, idVendor=vendor, idProduct=product)
        board.bootloader = True
        return board

    @classmethod
    def from_usb(cls, baudrate=115200, wait=0, timeout=10, **kwargs):
        import usb.core

        dev = usb.core.find(**kwargs)
        if not dev:
            s = "Can't find USB device: "
            args = []
            for x in kwargs.items():
                try:
                    args.append("%s=0x%x" % x)
                except:
                    args.append("%s = %s" % x)
            raise RuntimeError("Can't find USB device: " + ", ".join(args))
        return cls(dev, baudrate=baudrate, wait=wait, timeout=timeout)

    def __init__(self, device, baudrate=115200, wait=0, timeout=10):
        self.device = device
        self.usb_dev = None
        try:
            # Is it a usb.core.Device?
            portstr = ":" + ".".join(map(str, device.port_numbers)) + ":"
        except:
            pass
        else:
            serials = [serial for serial in os.listdir("/dev/serial/by-path") if portstr in serial]
            if len(serials) != 1:
                raise RuntimeError("Can't find excatly one matching usb serial device")
            self.device = os.path.realpath("/dev/serial/by-path/" + serials[0])
            self.usb_dev = device

        self.baudrate = baudrate
        self.wait = wait
        self.timeout = timeout
        self.debug = True
        self.mount = None
        self.serial = None
        self.bootloader = False
        self.repl = REPL(self)

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        self.close()

    def open(self, baudrate=None, wait=None):
        if self.serial:
            return
        if baudrate is None:
            baudrate = self.baudrate
        if wait is None:
            wait = self.wait

        delayed = False
        for attempt in range(wait + 1):
            try:
                self.serial = serial.Serial(
                    self.device,
                    baudrate=self.baudrate,
                    timeout=self.timeout,
                    inter_byte_timeout=10,
                    write_timeout=self.timeout,
                )
                break
            except (OSError, IOError):  # Py2 and Py3 have different errors
                if wait == 0:
                    continue
                if attempt == 0:
                    sys.stdout.write("Waiting {} seconds for board ".format(wait))
                    delayed = True
            time.sleep(1)
            sys.stdout.write(".")
            sys.stdout.flush()
        else:
            if delayed:
                print("")
            raise CPboardError("failed to access " + self.device)
        if delayed:
            print("")

    def close(self):
        if self.serial:
            self.serial.close()
            self.serial = None

    def exec(self, command, timeout=10, wait_for_response=True):
        with self.repl as repl:
            try:
                output, error = repl.execute(
                    command, timeout=timeout, wait_for_response=wait_for_response
                )
            except OSError as e:
                if self.debug:
                    print("exec: session: ", self.repl.session)
                raise CPboardError("timeout", e)
        if error:
            raise CPboardError("exception", output, error)
        return output

    def eval(self, expression, timeout=10):
        command = "print({})".format(expression)
        with self.repl as repl:
            output, error = repl.execute(command, timeout=timeout)
        if error:
            raise CPboardError("exception", output, error)
        try:
            res = eval(str(output, encoding="utf8"))
        except:
            raise CPboardError("failed to eval: %s" % output)
        return res

    def _reset(self, mode="NORMAL"):
        self.exec(
            "import microcontroller;microcontroller.on_next_reset(microcontroller.RunMode.%s)"
            % mode
        )
        try:
            self.exec("import microcontroller;microcontroller.reset()", wait_for_response=False)
        except OSError:
            pass

    def reset(self, safe_mode=False, delay=5, wait=10):
        self._reset("SAFE_MODE" if safe_mode else "NORMAL")
        self.close()
        time.sleep(delay)
        self.open(wait)
        time.sleep(delay)

    def reset_to_bootloader(self, repl=False):
        if repl:
            self._reset("BOOTLOADER")
            self.close()
        else:
            self.close()
            s = serial.Serial(self.device, 1200, write_timeout=4, timeout=4)
            s.close()

    def get_port_info(self):
        portinfo = None
        for port_iter in serial.tools.list_ports.comports():
            if port_iter.device == self.device:
                portinfo = port_iter
                break
        return portinfo

    @property
    def serial_number(self):
        try:  # Permissions are needed to read the value
            return self.usb_dev.serial_number
        except:
            pass
        p = self.get_port_info()
        return p.serial_number if p else None

    def get_disks(self):
        if self.usb_dev:
            portstr = ":" + ".".join(map(str, self.usb_dev.port_numbers)) + ":"
            return [
                "/dev/disk/by-path/" + disk
                for disk in os.listdir("/dev/disk/by-path")
                if portstr in disk
            ]
        serial = self.serial_number
        if not serial:
            raise RuntimeError("Serial number not found for: " + self.device)
        return [
            "/dev/disk/by-id/" + disk for disk in os.listdir("/dev/disk/by-id") if serial in disk
        ]

    @property
    def disk(self):
        disks = self.get_disks()

        part = [part for part in disks if "part1" in part]
        if not part:
            raise RuntimeError("Disk not found for: " + self.device)

        return Disk(part[0])

    @property
    def firmware(self):
        return Firmware(self)

    def execfile_disk(self, filename):
        with self.disk as disk:
            disk.copy(filename, "code.py")

        with self.repl as repl:
            try:
                output = repl.run()
            except OSError as e:
                raise CPboardError("timeout", e)
            except RuntimeError:
                if self.repl.safe_mode:
                    raise PyboardError("Can't run in safe mode")
                else:
                    raise

        return output

    def execfile(self, filename, timeout=10):
        if os.environ.get("CPBOARD_EXEC_MODE") == "disk":
            return self.execfile_disk(filename)
        else:
            with open(filename, "rb") as f:
                pyfile = f.read()
            return self.exec(pyfile, timeout=timeout)


# Implement just enough to make tests/run-tests work
PyboardError = CPboardError


class Pyboard:
    def __init__(self, device, baudrate=115200, user="micro", password="python", wait=0):
        self.board = CPboard.from_try_all(device, baudrate=baudrate, wait=wait)
        with self.board.disk as disk:
            disk.copy("skip_if.py")

    def close(self):
        self.board.close()

    def enter_raw_repl(self):
        self.board.open()

    def execfile(self, filename):
        return self.board.execfile(filename)


def eval_namedtuple(board, command):
    from collections import namedtuple

    s = board.exec("print(%s)" % command)
    s = s.decode().strip()
    items = [key.split("=") for key in s[1:-1].split(", ")]
    keys = [item[0] for item in items]
    vals = [item[1] for item in items]
    nt = namedtuple("eval", keys)
    res = nt(*[eval(val) for val in vals])
    return res


def os_uname(board):
    return eval_namedtuple(board, "__import__('os').uname()")


def print_verbose(cargs, *args, **kwargs):
    if cargs.verbose:
        print(*args, flush=True, **kwargs)


def upload(args):
    try:
        board = CPboard.from_build_name_bootloader(args.board)
        print_verbose(args, "Board is already in the bootloader")
    except (ValueError, RuntimeError):
        board = CPboard.from_try_all(args.board)

    print_verbose(args, "Serial number :", board.serial_number)

    if not (args.quiet or board.bootloader):
        board.open()
        print("Current version:", os_uname(board).version, flush=True)

    if not board.bootloader:
        print_verbose(args, "Reset to bootloader...", end="")
        board.reset_to_bootloader(repl=True)  # Feather M0 Express doesn't respond to 1200 baud
        time.sleep(5)
        print_verbose(args, "done")

    print_verbose(args, "Bootloader:", board.firmware.info)

    print_verbose(args, "Upload firmware...", end="")
    board.firmware.upload(args.firmware)
    print_verbose(args, "done")

    print_verbose(args, "Wait for board...", end="")
    time.sleep(5)
    print_verbose(args, "done")

    if not args.quiet:
        if board.bootloader:
            board = CPboard.from_try_all(args.board)
        board.open(wait=10)
        print("New version:", os_uname(board).version, flush=True)


def print_error_exit(args, e):
    if args.debug:
        return False
    if not args.quiet:
        print(e, file=sys.stderr)
    sys.exit(1)


def main():
    import argparse

    cmd_parser = argparse.ArgumentParser(description="CircuitPython Board Tool")
    cmd_parser.add_argument("board", help="build_name, vid:pid or /dev/tty")
    cmd_parser.add_argument("-f", "--firmware", help="upload UF2 firmware file")
    cmd_parser.add_argument("-c", "--command", help="program passed in as string")
    cmd_parser.add_argument("--tty", action="store_true", help="print tty")
    cmd_parser.add_argument("--verbose", "-v", action="count", default=0, help="be verbose")
    cmd_parser.add_argument("-q", "--quiet", action="store_true", help="be quiet")
    cmd_parser.add_argument("--debug", action="store_true", help="raise exceptions")
    args = cmd_parser.parse_args()

    if args.quiet:
        args.verbose = 0
        args.debug = False

    if args.firmware:
        try:
            upload(args)
        except BaseException as e:
            if not print_error_exit(args, e):
                raise
        sys.exit(0)

    try:
        board = CPboard.from_try_all(args.board)
    except BaseException as e:
        if not print_error_exit(args, e):
            raise

    if args.verbose:
        exec_mode = os.environ.get("CPBOARD_EXEC_MODE")
        if exec_mode:
            print("CPBOARD_EXEC_MODE =", exec_mode)

    # Make sure we can open serial
    try:
        with board:
            pass
    except BaseException as e:
        if not print_error_exit(args, e):
            raise

    if args.tty:
        print(board.device)
    elif args.command:
        with board as b:
            print(b.eval(args.command))
    else:
        with board as b:
            print("Device: ", end="")
            if b.usb_dev:
                print("%04x:%04x on " % (b.usb_dev.idVendor, b.usb_dev.idProduct), end="")
            print(b.device)
            print("Serial number:", b.serial_number)
            uname = os_uname(b)
            print("os.uname:")
            print("  sysname:", uname.sysname)
            print("  nodename:", uname.nodename)
            print("  release:", uname.release)
            print("  version:", uname.version)
            print("  machine:", uname.machine)


if __name__ == "__main__":
    main()
