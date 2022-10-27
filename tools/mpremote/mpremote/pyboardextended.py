import io, os, re, serial, struct, time
from errno import EPERM
from .console import VT_ENABLED

try:
    from .pyboard import Pyboard, PyboardError, stdout_write_bytes, filesystem_command
except ImportError:
    import sys

    sys.path.append(os.path.dirname(__file__) + "/../..")
    from pyboard import Pyboard, PyboardError, stdout_write_bytes, filesystem_command

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
import uos, uio, ustruct, micropython

SEEK_SET = 0

class RemoteCommand:
    def __init__(self):
        import uselect, usys
        self.buf4 = bytearray(4)
        self.fout = usys.stdout.buffer
        self.fin = usys.stdin.buffer
        self.poller = uselect.poll()
        self.poller.register(self.fin, uselect.POLLIN)

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
        ustruct.pack_into('<i', self.buf4, 0, i)
        self.fout.write(self.buf4)

    def wr_bytes(self, b):
        self.wr_s32(len(b))
        self.fout.write(b)

    # str and bytes act the same in MicroPython
    wr_str = wr_bytes


class RemoteFile(uio.IOBase):
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
        c.wr_str(self.path + path)
        res = c.rd_s32()
        c.end()
        if res < 0:
            raise OSError(-res)

    def rename(self, old, new):
        c = self.cmd
        c.begin(CMD_RENAME)
        c.wr_str(self.path + old)
        c.wr_str(self.path + new)
        res = c.rd_s32()
        c.end()
        if res < 0:
            raise OSError(-res)

    def mkdir(self, path):
        c = self.cmd
        c.begin(CMD_MKDIR)
        c.wr_str(self.path + path)
        res = c.rd_s32()
        c.end()
        if res < 0:
            raise OSError(-res)

    def rmdir(self, path):
        c = self.cmd
        c.begin(CMD_RMDIR)
        c.wr_str(self.path + path)
        res = c.rd_s32()
        c.end()
        if res < 0:
            raise OSError(-res)

    def stat(self, path):
        c = self.cmd
        c.begin(CMD_STAT)
        c.wr_str(self.path + path)
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
        c.wr_str(self.path + path)
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
        c.wr_str(self.path + path)
        c.wr_str(mode)
        fd = c.rd_s8()
        c.end()
        if fd < 0:
            raise OSError(-fd)
        return RemoteFile(c, fd, mode.find('b') == -1)


def __mount():
    uos.mount(RemoteFS(RemoteCommand()), '/remote')
    uos.chdir('/remote')
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
            except OSError as er:
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


class PyboardExtended(Pyboard):
    def __init__(self, dev, *args, **kwargs):
        super().__init__(dev, *args, **kwargs)
        self.device_name = dev
        self.mounted = False

    def mount_local(self, path, unsafe_links=False):
        fout = self.serial
        if self.eval('"RemoteFS" in globals()') == b"False":
            self.exec_(fs_hook_code)
        self.exec_("__mount()")
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
        self.exec_(fs_hook_code)
        self.exec_("__mount()")
        self.mounted = True

        # Exit raw REPL if needed, and wait for the friendly REPL prompt.
        if in_friendly_repl:
            self.exit_raw_repl()
        self.read_until(len(prompt), prompt)
        out_callback(prompt)
        self.serial = SerialIntercept(self.serial, self.cmd)

    def umount_local(self):
        if self.mounted:
            self.exec_('uos.umount("/remote")')
            self.mounted = False
            self.serial = self.serial.orig_serial
