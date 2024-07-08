#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2014-2021 Damien P. George
# Copyright (c) 2017 Paul Sokolovsky
# Copyright (c) 2023 Jim Mussared
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

# This is based on the serial-only parts of tools/pyboard.py, with Python 2
# support removed, and is currently in the process of being refactored to
# support multiple transports (webrepl, socket, BLE, etc). At the moment,
# SerialTransport is just the old Pyboard+PyboardExtended class without any
# of this refactoring. The API is going to change significantly.

# Once the API is stabilised, the idea is that mpremote can be used both
# as a command line tool and a library for interacting with devices.

import ast, io, errno, os, re, struct, sys, time
from collections import namedtuple
from errno import EPERM
from .console import VT_ENABLED
from .transport import TransportError, Transport


def stdout_write_bytes(b):
    b = b.replace(b"\x04", b"")
    sys.stdout.buffer.write(b)
    sys.stdout.buffer.flush()


listdir_result = namedtuple("dir_result", ["name", "st_mode", "st_ino", "st_size"])


def reraise_filesystem_error(e, info):
    if len(e.args) >= 3:
        if b"OSError" in e.args[2] and b"ENOENT" in e.args[2]:
            raise FileNotFoundError(info)
    raise


class SerialTransport(Transport):
    def __init__(self, device, baudrate=115200, wait=0, exclusive=True):
        self.in_raw_repl = False
        self.use_raw_paste = True
        self.device_name = device
        self.mounted = False

        import serial
        import serial.tools.list_ports

        # Set options, and exclusive if pyserial supports it
        serial_kwargs = {"baudrate": baudrate, "interCharTimeout": 1}
        if serial.__version__ >= "3.3":
            serial_kwargs["exclusive"] = exclusive

        delayed = False
        for attempt in range(wait + 1):
            try:
                if device.startswith("rfc2217://"):
                    self.serial = serial.serial_for_url(device, **serial_kwargs)
                elif os.name == "nt":
                    self.serial = serial.Serial(**serial_kwargs)
                    self.serial.port = device
                    portinfo = list(serial.tools.list_ports.grep(device))  # type: ignore
                    if portinfo and portinfo[0].manufacturer != "Microsoft":
                        # ESP8266/ESP32 boards use RTS/CTS for flashing and boot mode selection.
                        # DTR False: to avoid using the reset button will hang the MCU in bootloader mode
                        # RTS False: to prevent pulses on rts on serial.close() that would POWERON_RESET an ESPxx
                        self.serial.dtr = False  # DTR False = gpio0 High = Normal boot
                        self.serial.rts = False  # RTS False = EN High = MCU enabled
                    self.serial.open()
                else:
                    self.serial = serial.Serial(device, **serial_kwargs)
                break
            except OSError:
                if wait == 0:
                    continue
                if attempt == 0:
                    sys.stdout.write("Waiting {} seconds for pyboard ".format(wait))
                    delayed = True
            time.sleep(1)
            sys.stdout.write(".")
            sys.stdout.flush()
        else:
            if delayed:
                print("")
            raise TransportError("failed to access " + device)
        if delayed:
            print("")

    def close(self):
        self.serial.close()

    def read_until(self, min_num_bytes, ending, timeout=10, data_consumer=None):
        # if data_consumer is used then data is not accumulated and the ending must be 1 byte long
        assert data_consumer is None or len(ending) == 1

        data = self.serial.read(min_num_bytes)
        if data_consumer:
            data_consumer(data)
        timeout_count = 0
        while True:
            if data.endswith(ending):
                break
            elif self.serial.inWaiting() > 0:
                new_data = self.serial.read(1)
                if data_consumer:
                    data_consumer(new_data)
                    data = new_data
                else:
                    data = data + new_data
                timeout_count = 0
            else:
                timeout_count += 1
                if timeout is not None and timeout_count >= 100 * timeout:
                    break
                time.sleep(0.01)
        return data

    def enter_raw_repl(self, soft_reset=True):
        self.serial.write(b"\r\x03\x03")  # ctrl-C twice: interrupt any running program

        # flush input (without relying on serial.flushInput())
        n = self.serial.inWaiting()
        while n > 0:
            self.serial.read(n)
            n = self.serial.inWaiting()

        self.serial.write(b"\r\x01")  # ctrl-A: enter raw REPL

        if soft_reset:
            data = self.read_until(1, b"raw REPL; CTRL-B to exit\r\n>")
            if not data.endswith(b"raw REPL; CTRL-B to exit\r\n>"):
                print(data)
                raise TransportError("could not enter raw repl")

            self.serial.write(b"\x04")  # ctrl-D: soft reset

            # Waiting for "soft reboot" independently to "raw REPL" (done below)
            # allows boot.py to print, which will show up after "soft reboot"
            # and before "raw REPL".
            data = self.read_until(1, b"soft reboot\r\n")
            if not data.endswith(b"soft reboot\r\n"):
                print(data)
                raise TransportError("could not enter raw repl")

        data = self.read_until(1, b"raw REPL; CTRL-B to exit\r\n")
        if not data.endswith(b"raw REPL; CTRL-B to exit\r\n"):
            print(data)
            raise TransportError("could not enter raw repl")

        self.in_raw_repl = True

    def exit_raw_repl(self):
        self.serial.write(b"\r\x02")  # ctrl-B: enter friendly REPL
        self.in_raw_repl = False

    def follow(self, timeout, data_consumer=None):
        # wait for normal output
        data = self.read_until(1, b"\x04", timeout=timeout, data_consumer=data_consumer)
        if not data.endswith(b"\x04"):
            raise TransportError("timeout waiting for first EOF reception")
        data = data[:-1]

        # wait for error output
        data_err = self.read_until(1, b"\x04", timeout=timeout)
        if not data_err.endswith(b"\x04"):
            raise TransportError("timeout waiting for second EOF reception")
        data_err = data_err[:-1]

        # return normal and error output
        return data, data_err

    def raw_paste_write(self, command_bytes):
        # Read initial header, with window size.
        data = self.serial.read(2)
        window_size = struct.unpack("<H", data)[0]
        window_remain = window_size

        # Write out the command_bytes data.
        i = 0
        while i < len(command_bytes):
            while window_remain == 0 or self.serial.inWaiting():
                data = self.serial.read(1)
                if data == b"\x01":
                    # Device indicated that a new window of data can be sent.
                    window_remain += window_size
                elif data == b"\x04":
                    # Device indicated abrupt end.  Acknowledge it and finish.
                    self.serial.write(b"\x04")
                    return
                else:
                    # Unexpected data from device.
                    raise TransportError("unexpected read during raw paste: {}".format(data))
            # Send out as much data as possible that fits within the allowed window.
            b = command_bytes[i : min(i + window_remain, len(command_bytes))]
            self.serial.write(b)
            window_remain -= len(b)
            i += len(b)

        # Indicate end of data.
        self.serial.write(b"\x04")

        # Wait for device to acknowledge end of data.
        data = self.read_until(1, b"\x04")
        if not data.endswith(b"\x04"):
            raise TransportError("could not complete raw paste: {}".format(data))

    def exec_raw_no_follow(self, command):
        if isinstance(command, bytes):
            command_bytes = command
        else:
            command_bytes = bytes(command, encoding="utf8")

        # check we have a prompt
        data = self.read_until(1, b">")
        if not data.endswith(b">"):
            raise TransportError("could not enter raw repl")

        if self.use_raw_paste:
            # Try to enter raw-paste mode.
            self.serial.write(b"\x05A\x01")
            data = self.serial.read(2)
            if data == b"R\x00":
                # Device understood raw-paste command but doesn't support it.
                pass
            elif data == b"R\x01":
                # Device supports raw-paste mode, write out the command using this mode.
                return self.raw_paste_write(command_bytes)
            else:
                # Device doesn't support raw-paste, fall back to normal raw REPL.
                data = self.read_until(1, b"w REPL; CTRL-B to exit\r\n>")
                if not data.endswith(b"w REPL; CTRL-B to exit\r\n>"):
                    print(data)
                    raise TransportError("could not enter raw repl")
            # Don't try to use raw-paste mode again for this connection.
            self.use_raw_paste = False

        # Write command using standard raw REPL, 256 bytes every 10ms.
        for i in range(0, len(command_bytes), 256):
            self.serial.write(command_bytes[i : min(i + 256, len(command_bytes))])
            time.sleep(0.01)
        self.serial.write(b"\x04")

        # check if we could exec command
        data = self.serial.read(2)
        if data != b"OK":
            raise TransportError("could not exec command (response: %r)" % data)

    def exec_raw(self, command, timeout=10, data_consumer=None):
        self.exec_raw_no_follow(command)
        return self.follow(timeout, data_consumer)

    def eval(self, expression, parse=False):
        if parse:
            ret = self.exec("print(repr({}))".format(expression))
            ret = ret.strip()
            return ast.literal_eval(ret.decode())
        else:
            ret = self.exec("print({})".format(expression))
            ret = ret.strip()
            return ret

    def exec(self, command, data_consumer=None):
        ret, ret_err = self.exec_raw(command, data_consumer=data_consumer)
        if ret_err:
            raise TransportError("exception", ret, ret_err)
        return ret

    def execfile(self, filename):
        with open(filename, "rb") as f:
            pyfile = f.read()
        return self.exec(pyfile)

    def fs_exists(self, src):
        try:
            self.exec("import os\nos.stat(%s)" % (("'%s'" % src) if src else ""))
            return True
        except TransportError:
            return False

    def fs_ls(self, src):
        cmd = (
            "import os\nfor f in os.ilistdir(%s):\n"
            " print('{:12} {}{}'.format(f[3]if len(f)>3 else 0,f[0],'/'if f[1]&0x4000 else ''))"
            % (("'%s'" % src) if src else "")
        )
        self.exec(cmd, data_consumer=stdout_write_bytes)

    def fs_listdir(self, src=""):
        buf = bytearray()

        def repr_consumer(b):
            buf.extend(b.replace(b"\x04", b""))

        cmd = "import os\nfor f in os.ilistdir(%s):\n" " print(repr(f), end=',')" % (
            ("'%s'" % src) if src else ""
        )
        try:
            buf.extend(b"[")
            self.exec(cmd, data_consumer=repr_consumer)
            buf.extend(b"]")
        except TransportError as e:
            reraise_filesystem_error(e, src)

        return [
            listdir_result(*f) if len(f) == 4 else listdir_result(*(f + (0,)))
            for f in ast.literal_eval(buf.decode())
        ]

    def fs_stat(self, src):
        try:
            self.exec("import os")
            return os.stat_result(self.eval("os.stat(%s)" % ("'%s'" % src), parse=True))
        except TransportError as e:
            reraise_filesystem_error(e, src)

    def fs_cat(self, src, chunk_size=256):
        cmd = (
            "with open('%s') as f:\n while 1:\n"
            "  b=f.read(%u)\n  if not b:break\n  print(b,end='')" % (src, chunk_size)
        )
        self.exec(cmd, data_consumer=stdout_write_bytes)

    def fs_readfile(self, src, chunk_size=256):
        buf = bytearray()

        def repr_consumer(b):
            buf.extend(b.replace(b"\x04", b""))

        cmd = (
            "with open('%s', 'rb') as f:\n while 1:\n"
            "  b=f.read(%u)\n  if not b:break\n  print(b,end='')" % (src, chunk_size)
        )
        try:
            self.exec(cmd, data_consumer=repr_consumer)
        except TransportError as e:
            reraise_filesystem_error(e, src)
        return ast.literal_eval(buf.decode())

    def fs_writefile(self, dest, data, chunk_size=256):
        self.exec("f=open('%s','wb')\nw=f.write" % dest)
        while data:
            chunk = data[:chunk_size]
            self.exec("w(" + repr(chunk) + ")")
            data = data[len(chunk) :]
        self.exec("f.close()")

    def fs_cp(self, src, dest, chunk_size=256, progress_callback=None):
        if progress_callback:
            src_size = self.fs_stat(src).st_size
            written = 0
        self.exec("fr=open('%s','rb')\nr=fr.read\nfw=open('%s','wb')\nw=fw.write" % (src, dest))
        while True:
            data_len = int(self.exec("d=r(%u)\nw(d)\nprint(len(d))" % chunk_size))
            if not data_len:
                break
            if progress_callback:
                written += data_len
                progress_callback(written, src_size)
        self.exec("fr.close()\nfw.close()")

    def fs_get(self, src, dest, chunk_size=256, progress_callback=None):
        if progress_callback:
            src_size = self.fs_stat(src).st_size
            written = 0
        self.exec("f=open('%s','rb')\nr=f.read" % src)
        with open(dest, "wb") as f:
            while True:
                data = bytearray()
                self.exec("print(r(%u))" % chunk_size, data_consumer=lambda d: data.extend(d))
                assert data.endswith(b"\r\n\x04")
                try:
                    data = ast.literal_eval(str(data[:-3], "ascii"))
                    if not isinstance(data, bytes):
                        raise ValueError("Not bytes")
                except (UnicodeError, ValueError) as e:
                    raise TransportError("fs_get: Could not interpret received data: %s" % str(e))
                if not data:
                    break
                f.write(data)
                if progress_callback:
                    written += len(data)
                    progress_callback(written, src_size)
        self.exec("f.close()")

    def fs_put(self, src, dest, chunk_size=256, progress_callback=None):
        if progress_callback:
            src_size = os.path.getsize(src)
            written = 0
        self.exec("f=open('%s','wb')\nw=f.write" % dest)
        with open(src, "rb") as f:
            while True:
                data = f.read(chunk_size)
                if not data:
                    break
                if sys.version_info < (3,):
                    self.exec("w(b" + repr(data) + ")")
                else:
                    self.exec("w(" + repr(data) + ")")
                if progress_callback:
                    written += len(data)
                    progress_callback(written, src_size)
        self.exec("f.close()")

    def fs_mkdir(self, dir):
        self.exec("import os\nos.mkdir('%s')" % dir)

    def fs_rmdir(self, dir):
        self.exec("import os\nos.rmdir('%s')" % dir)

    def fs_rm(self, src):
        self.exec("import os\nos.remove('%s')" % src)

    def fs_touch(self, src):
        self.exec("f=open('%s','a')\nf.close()" % src)

    def filesystem_command(self, args, progress_callback=None, verbose=False):
        def fname_remote(src):
            if src.startswith(":"):
                src = src[1:]
            # Convert all path separators to "/", because that's what a remote device uses.
            return src.replace(os.path.sep, "/")

        def fname_cp_dest(src, dest):
            _, src = os.path.split(src)
            if dest is None or dest == "":
                dest = src
            elif dest == ".":
                dest = "./" + src
            elif dest.endswith("/"):
                dest += src
            return dest

        cmd = args[0]
        args = args[1:]
        try:
            if cmd == "cp":
                srcs = args[:-1]
                dest = args[-1]
                if dest.startswith(":"):
                    op_remote_src = self.fs_cp
                    op_local_src = self.fs_put
                else:
                    op_remote_src = self.fs_get
                    op_local_src = lambda src, dest, **_: __import__("shutil").copy(src, dest)
                for src in srcs:
                    if verbose:
                        print("cp %s %s" % (src, dest))
                    if src.startswith(":"):
                        op = op_remote_src
                    else:
                        op = op_local_src
                    src2 = fname_remote(src)
                    dest2 = fname_cp_dest(src2, fname_remote(dest))
                    op(src2, dest2, progress_callback=progress_callback)
            else:
                ops = {
                    "cat": self.fs_cat,
                    "ls": self.fs_ls,
                    "mkdir": self.fs_mkdir,
                    "rm": self.fs_rm,
                    "rmdir": self.fs_rmdir,
                    "touch": self.fs_touch,
                }
                if cmd not in ops:
                    raise TransportError("'{}' is not a filesystem command".format(cmd))
                if cmd == "ls" and not args:
                    args = [""]
                for src in args:
                    src = fname_remote(src)
                    if verbose:
                        print("%s :%s" % (cmd, src))
                    ops[cmd](src)
        except TransportError as er:
            if len(er.args) > 1:
                print(str(er.args[2], "ascii"))
            else:
                print(er)
            self.exit_raw_repl()
            self.close()
            sys.exit(1)

    def mount_local(self, path, unsafe_links=False):
        fout = self.serial
        if self.eval('"RemoteFS" in globals()') == b"False":
            self.exec(fs_hook_code)
        self.exec("__mount()")
        self.mounted = True
        self.cmd = PyboardCommand(self.serial, fout, path, unsafe_links=unsafe_links)
        self.serial = SerialIntercept(self.serial, self.cmd)

    def write_ctrl_d(self, out_callback):
        self.serial.write(b"\x04")
        if not self.mounted:
            return

        # Read response from the device until it is quiet (with a timeout).
        INITIAL_TIMEOUT = 0.5
        BANNER_TIMEOUT = 2
        QUIET_TIMEOUT = 0.1
        FULL_TIMEOUT = 5
        t_start = t_last_activity = time.monotonic()
        data_all = b""
        soft_reboot_started = False
        soft_reboot_banner = False
        while True:
            t = time.monotonic()
            n = self.serial.inWaiting()
            if n > 0:
                data = self.serial.read(n)
                out_callback(data)
                data_all += data
                t_last_activity = t
            else:
                if len(data_all) == 0:
                    if t - t_start > INITIAL_TIMEOUT:
                        return
                else:
                    if t - t_start > FULL_TIMEOUT:
                        if soft_reboot_started:
                            break
                        return

                    next_data_timeout = QUIET_TIMEOUT

                    if not soft_reboot_started and data_all.find(b"MPY: soft reboot") != -1:
                        soft_reboot_started = True

                    if soft_reboot_started and not soft_reboot_banner:
                        # Once soft reboot has been initiated, give some more time for the startup
                        # banner to be shown
                        if data_all.find(b"\nMicroPython ") != -1:
                            soft_reboot_banner = True
                        elif data_all.find(b"\nraw REPL; CTRL-B to exit\r\n") != -1:
                            soft_reboot_banner = True
                        else:
                            next_data_timeout = BANNER_TIMEOUT

                    if t - t_last_activity > next_data_timeout:
                        break

        if not soft_reboot_started:
            return

        if not soft_reboot_banner:
            out_callback(b"Warning: Could not remount local filesystem\r\n")
            return

        # Determine type of prompt
        if data_all.endswith(b">"):
            in_friendly_repl = False
            prompt = b">"
        else:
            in_friendly_repl = True
            prompt = data_all.rsplit(b"\r\n", 1)[-1]

        # Clear state while board remounts, it will be re-set once mounted.
        self.mounted = False
        self.serial = self.serial.orig_serial

        # Provide a message about the remount.
        out_callback(bytes(f"\r\nRemount local directory {self.cmd.root} at /remote\r\n", "utf8"))

        # Enter raw REPL and re-mount the remote filesystem.
        self.serial.write(b"\x01")
        self.exec(fs_hook_code)
        self.exec("__mount()")
        self.mounted = True

        # Exit raw REPL if needed, and wait for the friendly REPL prompt.
        if in_friendly_repl:
            self.exit_raw_repl()
        self.read_until(len(prompt), prompt)
        out_callback(prompt)
        self.serial = SerialIntercept(self.serial, self.cmd)

    def umount_local(self):
        if self.mounted:
            self.exec('os.umount("/remote")')
            self.mounted = False
            self.serial = self.serial.orig_serial


fs_hook_cmds = {
    "CMD_STAT": 1,
    "CMD_ILISTDIR_START": 2,
    "CMD_ILISTDIR_NEXT": 3,
    "CMD_OPEN": 4,
    "CMD_CLOSE": 5,
    "CMD_READ": 6,
    "CMD_WRITE": 7,
    "CMD_SEEK": 8,
    "CMD_REMOVE": 9,
    "CMD_RENAME": 10,
    "CMD_MKDIR": 11,
    "CMD_RMDIR": 12,
}

fs_hook_code = """\
import os, io, struct, micropython

SEEK_SET = 0

class RemoteCommand:
    def __init__(self):
        import select, sys
        self.buf4 = bytearray(4)
        self.fout = sys.stdout.buffer
        self.fin = sys.stdin.buffer
        self.poller = select.poll()
        self.poller.register(self.fin, select.POLLIN)

    def poll_in(self):
        for _ in self.poller.ipoll(1000):
            return
        self.end()
        raise Exception('timeout waiting for remote')

    def rd(self, n):
        buf = bytearray(n)
        self.rd_into(buf, n)
        return buf

    def rd_into(self, buf, n):
        # implement reading with a timeout in case other side disappears
        if n == 0:
            return
        self.poll_in()
        r = self.fin.readinto(buf, n)
        if r < n:
            mv = memoryview(buf)
            while r < n:
                self.poll_in()
                r += self.fin.readinto(mv[r:], n - r)

    def begin(self, type):
        micropython.kbd_intr(-1)
        buf4 = self.buf4
        buf4[0] = 0x18
        buf4[1] = type
        self.fout.write(buf4, 2)
        # Wait for sync byte 0x18, but don't get stuck forever
        for i in range(30):
            self.poller.poll(1000)
            self.fin.readinto(buf4, 1)
            if buf4[0] == 0x18:
                break

    def end(self):
        micropython.kbd_intr(3)

    def rd_s8(self):
        self.rd_into(self.buf4, 1)
        n = self.buf4[0]
        if n & 0x80:
            n -= 0x100
        return n

    def rd_s32(self):
        buf4 = self.buf4
        self.rd_into(buf4, 4)
        n = buf4[0] | buf4[1] << 8 | buf4[2] << 16 | buf4[3] << 24
        if buf4[3] & 0x80:
            n -= 0x100000000
        return n

    def rd_u32(self):
        buf4 = self.buf4
        self.rd_into(buf4, 4)
        return buf4[0] | buf4[1] << 8 | buf4[2] << 16 | buf4[3] << 24

    def rd_bytes(self, buf):
        # TODO if n is large (eg >256) then we may miss bytes on stdin
        n = self.rd_s32()
        if buf is None:
            ret = buf = bytearray(n)
        else:
            ret = n
        self.rd_into(buf, n)
        return ret

    def rd_str(self):
        n = self.rd_s32()
        if n == 0:
            return ''
        else:
            return str(self.rd(n), 'utf8')

    def wr_s8(self, i):
        self.buf4[0] = i
        self.fout.write(self.buf4, 1)

    def wr_s32(self, i):
        struct.pack_into('<i', self.buf4, 0, i)
        self.fout.write(self.buf4)

    def wr_bytes(self, b):
        self.wr_s32(len(b))
        self.fout.write(b)

    # str and bytes act the same in MicroPython
    wr_str = wr_bytes


class RemoteFile(io.IOBase):
    def __init__(self, cmd, fd, is_text):
        self.cmd = cmd
        self.fd = fd
        self.is_text = is_text

    def __enter__(self):
        return self

    def __exit__(self, a, b, c):
        self.close()

    def __iter__(self):
        return self

    def __next__(self):
        l = self.readline()
        if not l:
            raise StopIteration
        return l

    def ioctl(self, request, arg):
        if request == 1:  # FLUSH
            self.flush()
        elif request == 2:  # SEEK
            # This assumes a 32-bit bare-metal machine.
            import machine
            machine.mem32[arg] = self.seek(machine.mem32[arg], machine.mem32[arg + 4])
        elif request == 4:  # CLOSE
            self.close()
        elif request == 11:  # BUFFER_SIZE
            # This is used as the vfs_reader buffer. n + 4 should be less than 255 to
            # fit in stdin ringbuffer on supported ports. n + 7 should be multiple of 16
            # to efficiently use gc blocks in mp_reader_vfs_t.
            return 249
        else:
            return -1
        return 0

    def flush(self):
        pass

    def close(self):
        if self.fd is None:
            return
        c = self.cmd
        c.begin(CMD_CLOSE)
        c.wr_s8(self.fd)
        c.end()
        self.fd = None

    def read(self, n=-1):
        c = self.cmd
        c.begin(CMD_READ)
        c.wr_s8(self.fd)
        c.wr_s32(n)
        data = c.rd_bytes(None)
        c.end()
        if self.is_text:
            data = str(data, 'utf8')
        else:
            data = bytes(data)
        return data

    def readinto(self, buf):
        c = self.cmd
        c.begin(CMD_READ)
        c.wr_s8(self.fd)
        c.wr_s32(len(buf))
        n = c.rd_bytes(buf)
        c.end()
        return n

    def readline(self):
        l = ''
        while 1:
            c = self.read(1)
            l += c
            if c == '\\n' or c == '':
                return l

    def readlines(self):
        ls = []
        while 1:
            l = self.readline()
            if not l:
                return ls
            ls.append(l)

    def write(self, buf):
        c = self.cmd
        c.begin(CMD_WRITE)
        c.wr_s8(self.fd)
        c.wr_bytes(buf)
        n = c.rd_s32()
        c.end()
        return n

    def seek(self, n, whence=SEEK_SET):
        c = self.cmd
        c.begin(CMD_SEEK)
        c.wr_s8(self.fd)
        c.wr_s32(n)
        c.wr_s8(whence)
        n = c.rd_s32()
        c.end()
        if n < 0:
            raise OSError(n)
        return n


class RemoteFS:
    def __init__(self, cmd):
        self.cmd = cmd

    def _abspath(self, path):
        return path if path.startswith("/") else self.path + path

    def mount(self, readonly, mkfs):
        pass

    def umount(self):
        pass

    def chdir(self, path):
        if not path.startswith("/"):
            path = self.path + path
        if not path.endswith("/"):
            path += "/"
        if path != "/":
            self.stat(path)
        self.path = path

    def getcwd(self):
        return self.path

    def remove(self, path):
        c = self.cmd
        c.begin(CMD_REMOVE)
        c.wr_str(self._abspath(path))
        res = c.rd_s32()
        c.end()
        if res < 0:
            raise OSError(-res)

    def rename(self, old, new):
        c = self.cmd
        c.begin(CMD_RENAME)
        c.wr_str(self._abspath(old))
        c.wr_str(self._abspath(new))
        res = c.rd_s32()
        c.end()
        if res < 0:
            raise OSError(-res)

    def mkdir(self, path):
        c = self.cmd
        c.begin(CMD_MKDIR)
        c.wr_str(self._abspath(path))
        res = c.rd_s32()
        c.end()
        if res < 0:
            raise OSError(-res)

    def rmdir(self, path):
        c = self.cmd
        c.begin(CMD_RMDIR)
        c.wr_str(self._abspath(path))
        res = c.rd_s32()
        c.end()
        if res < 0:
            raise OSError(-res)

    def stat(self, path):
        c = self.cmd
        c.begin(CMD_STAT)
        c.wr_str(self._abspath(path))
        res = c.rd_s8()
        if res < 0:
            c.end()
            raise OSError(-res)
        mode = c.rd_u32()
        size = c.rd_u32()
        atime = c.rd_u32()
        mtime = c.rd_u32()
        ctime = c.rd_u32()
        c.end()
        return mode, 0, 0, 0, 0, 0, size, atime, mtime, ctime

    def ilistdir(self, path):
        c = self.cmd
        c.begin(CMD_ILISTDIR_START)
        c.wr_str(self._abspath(path))
        res = c.rd_s8()
        c.end()
        if res < 0:
            raise OSError(-res)
        def next():
            while True:
                c.begin(CMD_ILISTDIR_NEXT)
                name = c.rd_str()
                if name:
                    type = c.rd_u32()
                    c.end()
                    yield (name, type, 0)
                else:
                    c.end()
                    break
        return next()

    def open(self, path, mode):
        c = self.cmd
        c.begin(CMD_OPEN)
        c.wr_str(self._abspath(path))
        c.wr_str(mode)
        fd = c.rd_s8()
        c.end()
        if fd < 0:
            raise OSError(-fd)
        return RemoteFile(c, fd, mode.find('b') == -1)


def __mount():
    os.mount(RemoteFS(RemoteCommand()), '/remote')
    os.chdir('/remote')
"""

# Apply basic compression on hook code.
for key, value in fs_hook_cmds.items():
    fs_hook_code = re.sub(key, str(value), fs_hook_code)
fs_hook_code = re.sub(" *#.*$", "", fs_hook_code, flags=re.MULTILINE)
fs_hook_code = re.sub("\n\n+", "\n", fs_hook_code)
fs_hook_code = re.sub("    ", " ", fs_hook_code)
fs_hook_code = re.sub("rd_", "r", fs_hook_code)
fs_hook_code = re.sub("wr_", "w", fs_hook_code)
fs_hook_code = re.sub("buf4", "b4", fs_hook_code)


class PyboardCommand:
    def __init__(self, fin, fout, path, unsafe_links=False):
        self.fin = fin
        self.fout = fout
        self.root = path + "/"
        self.data_ilistdir = ["", []]
        self.data_files = []
        self.unsafe_links = unsafe_links

    def rd_s8(self):
        return struct.unpack("<b", self.fin.read(1))[0]

    def rd_s32(self):
        return struct.unpack("<i", self.fin.read(4))[0]

    def rd_bytes(self):
        n = self.rd_s32()
        return self.fin.read(n)

    def rd_str(self):
        n = self.rd_s32()
        if n == 0:
            return ""
        else:
            return str(self.fin.read(n), "utf8")

    def wr_s8(self, i):
        self.fout.write(struct.pack("<b", i))

    def wr_s32(self, i):
        self.fout.write(struct.pack("<i", i))

    def wr_u32(self, i):
        self.fout.write(struct.pack("<I", i))

    def wr_bytes(self, b):
        self.wr_s32(len(b))
        self.fout.write(b)

    def wr_str(self, s):
        b = bytes(s, "utf8")
        self.wr_s32(len(b))
        self.fout.write(b)

    def log_cmd(self, msg):
        print(f"[{msg}]", end="\r\n")

    def path_check(self, path):
        if not self.unsafe_links:
            parent = os.path.realpath(self.root)
            child = os.path.realpath(path)
        else:
            parent = os.path.abspath(self.root)
            child = os.path.abspath(path)
        if parent != os.path.commonpath([parent, child]):
            raise OSError(EPERM, "")  # File is outside mounted dir

    def do_stat(self):
        path = self.root + self.rd_str()
        # self.log_cmd(f"stat {path}")
        try:
            self.path_check(path)
            stat = os.stat(path)
        except OSError as er:
            self.wr_s8(-abs(er.errno))
        else:
            self.wr_s8(0)
            # Note: st_ino would need to be 64-bit if added here
            self.wr_u32(stat.st_mode)
            self.wr_u32(stat.st_size)
            self.wr_u32(int(stat.st_atime))
            self.wr_u32(int(stat.st_mtime))
            self.wr_u32(int(stat.st_ctime))

    def do_ilistdir_start(self):
        path = self.root + self.rd_str()
        try:
            self.path_check(path)
            self.data_ilistdir[0] = path
            self.data_ilistdir[1] = os.listdir(path)
            self.wr_s8(0)
        except OSError as er:
            self.wr_s8(-abs(er.errno))

    def do_ilistdir_next(self):
        if self.data_ilistdir[1]:
            entry = self.data_ilistdir[1].pop(0)
            try:
                stat = os.lstat(self.data_ilistdir[0] + "/" + entry)
                mode = stat.st_mode & 0xC000
            except OSError:
                mode = 0
            self.wr_str(entry)
            self.wr_u32(mode)
        else:
            self.wr_str("")

    def do_open(self):
        path = self.root + self.rd_str()
        mode = self.rd_str()
        # self.log_cmd(f"open {path} {mode}")
        try:
            self.path_check(path)
            f = open(path, mode)
        except OSError as er:
            self.wr_s8(-abs(er.errno))
        else:
            is_text = mode.find("b") == -1
            try:
                fd = self.data_files.index(None)
                self.data_files[fd] = (f, is_text)
            except ValueError:
                fd = len(self.data_files)
                self.data_files.append((f, is_text))
            self.wr_s8(fd)

    def do_close(self):
        fd = self.rd_s8()
        # self.log_cmd(f"close {fd}")
        self.data_files[fd][0].close()
        self.data_files[fd] = None

    def do_read(self):
        fd = self.rd_s8()
        n = self.rd_s32()
        buf = self.data_files[fd][0].read(n)
        if self.data_files[fd][1]:
            buf = bytes(buf, "utf8")
        self.wr_bytes(buf)
        # self.log_cmd(f"read {fd} {n} -> {len(buf)}")

    def do_seek(self):
        fd = self.rd_s8()
        n = self.rd_s32()
        whence = self.rd_s8()
        # self.log_cmd(f"seek {fd} {n}")
        try:
            n = self.data_files[fd][0].seek(n, whence)
        except io.UnsupportedOperation:
            n = -1
        self.wr_s32(n)

    def do_write(self):
        fd = self.rd_s8()
        buf = self.rd_bytes()
        if self.data_files[fd][1]:
            buf = str(buf, "utf8")
        n = self.data_files[fd][0].write(buf)
        self.wr_s32(n)
        # self.log_cmd(f"write {fd} {len(buf)} -> {n}")

    def do_remove(self):
        path = self.root + self.rd_str()
        # self.log_cmd(f"remove {path}")
        try:
            self.path_check(path)
            os.remove(path)
            ret = 0
        except OSError as er:
            ret = -abs(er.errno)
        self.wr_s32(ret)

    def do_rename(self):
        old = self.root + self.rd_str()
        new = self.root + self.rd_str()
        # self.log_cmd(f"rename {old} {new}")
        try:
            self.path_check(old)
            self.path_check(new)
            os.rename(old, new)
            ret = 0
        except OSError as er:
            ret = -abs(er.errno)
        self.wr_s32(ret)

    def do_mkdir(self):
        path = self.root + self.rd_str()
        # self.log_cmd(f"mkdir {path}")
        try:
            self.path_check(path)
            os.mkdir(path)
            ret = 0
        except OSError as er:
            ret = -abs(er.errno)
        self.wr_s32(ret)

    def do_rmdir(self):
        path = self.root + self.rd_str()
        # self.log_cmd(f"rmdir {path}")
        try:
            self.path_check(path)
            os.rmdir(path)
            ret = 0
        except OSError as er:
            ret = -abs(er.errno)
        self.wr_s32(ret)

    cmd_table = {
        fs_hook_cmds["CMD_STAT"]: do_stat,
        fs_hook_cmds["CMD_ILISTDIR_START"]: do_ilistdir_start,
        fs_hook_cmds["CMD_ILISTDIR_NEXT"]: do_ilistdir_next,
        fs_hook_cmds["CMD_OPEN"]: do_open,
        fs_hook_cmds["CMD_CLOSE"]: do_close,
        fs_hook_cmds["CMD_READ"]: do_read,
        fs_hook_cmds["CMD_WRITE"]: do_write,
        fs_hook_cmds["CMD_SEEK"]: do_seek,
        fs_hook_cmds["CMD_REMOVE"]: do_remove,
        fs_hook_cmds["CMD_RENAME"]: do_rename,
        fs_hook_cmds["CMD_MKDIR"]: do_mkdir,
        fs_hook_cmds["CMD_RMDIR"]: do_rmdir,
    }


class SerialIntercept:
    def __init__(self, serial, cmd):
        self.orig_serial = serial
        self.cmd = cmd
        self.buf = b""
        self.orig_serial.timeout = 5.0

    def _check_input(self, blocking):
        if blocking or self.orig_serial.inWaiting() > 0:
            c = self.orig_serial.read(1)
            if c == b"\x18":
                # a special command
                c = self.orig_serial.read(1)[0]
                self.orig_serial.write(b"\x18")  # Acknowledge command
                PyboardCommand.cmd_table[c](self.cmd)
            elif not VT_ENABLED and c == b"\x1b":
                # ESC code, ignore these on windows
                esctype = self.orig_serial.read(1)
                if esctype == b"[":  # CSI
                    while not (0x40 < self.orig_serial.read(1)[0] < 0x7E):
                        # Looking for "final byte" of escape sequence
                        pass
            else:
                self.buf += c

    @property
    def fd(self):
        return self.orig_serial.fd

    def close(self):
        self.orig_serial.close()

    def inWaiting(self):
        self._check_input(False)
        return len(self.buf)

    def read(self, n):
        while len(self.buf) < n:
            self._check_input(True)
        out = self.buf[:n]
        self.buf = self.buf[n:]
        return out

    def write(self, buf):
        self.orig_serial.write(buf)
