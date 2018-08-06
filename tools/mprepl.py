#!/usr/bin/env python3

"""
This script gives you a MicroPython REPL prompt and provides a hook on the target
board so that the current directory on the host is mounted on the board, at /remote.

Usage:
    ./mprepl.py [device]

If not specified, device will default to /dev/ttyACM0.

To quit from the REPL press Ctrl-X.
"""

import os
import sys
import time
import struct
import select
from pathlib import Path
try:
    import termios
    import select
except ImportError:
    termios = None
    select = None
    import msvcrt

tools=Path(__file__).parent
if tools not in sys.path:
    sys.path.append(tools)

import pyboard


CMD_STAT = 1
CMD_ILISTDIR_START = 2
CMD_ILISTDIR_NEXT = 3
CMD_OPEN = 4
CMD_CLOSE = 5
CMD_READ = 6
CMD_WRITE = 7
CMD_SEEK = 8

fs_hook_code = """\
import os, io, select, ustruct as struct, micropython
CMD_STAT = 1
CMD_ILISTDIR_START = 2
CMD_ILISTDIR_NEXT = 3
CMD_OPEN = 4
CMD_CLOSE = 5
CMD_READ = 6
CMD_WRITE = 7
CMD_SEEK = 8
class RemoteCommand:
    def __init__(self):
        try:
            import pyb
            self.fout = pyb.USB_VCP()
            self.fin = pyb.USB_VCP()
            self.can_poll = True
        except:
            import sys
            self.fout = sys.stdout.buffer
            self.fin = sys.stdin.buffer
            # TODO sys.stdio doesn't support polling
            self.can_poll = False
    def poll_in(self):
        if self.can_poll:
            res = select.select([self.fin], [], [], 1000)
            if not res[0]:
                raise Exception('timeout waiting for remote response')
    def rd(self, n):
        # implement reading with a timeout in case other side disappears
        self.poll_in()
        return self.fin.read(n)
    def rdinto(self, buf):
        # implement reading with a timeout in case other side disappears
        self.poll_in()
        return self.fin.readinto(buf)
    def begin(self, type):
        micropython.kbd_intr(-1)
        self.fout.write(bytearray([0x18, type]))
    def end(self):
        micropython.kbd_intr(3)
    def rd_uint32(self):
        return struct.unpack('<I', self.rd(4))[0]
    def wr_uint32(self, i):
        self.fout.write(struct.pack('<I', i))
    def rd_uint64(self):
        return struct.unpack('<Q', self.rd(8))[0]
    def wr_uint64(self, i):
        self.fout.write(struct.pack('<Q', i))
    def rd_int32(self):
        return struct.unpack('<i', self.rd(4))[0]
    def wr_int32(self, i):
        self.fout.write(struct.pack('<i', i))
    def rd_bytes(self):
        n = struct.unpack('<H', self.rd(2))[0]
        return self.rd(n)
    def rd_bytes_into(self, buf):
        n = struct.unpack('<H', self.rd(2))[0]
        return self.rdinto(memoryview(buf)[:n])
    def wr_bytes(self, b):
        self.fout.write(struct.pack('<H', len(b)))
        self.fout.write(b)
    def rd_str(self):
        n = self.rd(1)[0]
        if n == 0:
            return ''
        else:
            return str(self.rd(n), 'utf8')
    def wr_str(self, s):
        b = bytes(s, 'utf8')
        l = len(b)
        assert l <= 255
        self.fout.write(bytearray([l]) + b)

class RemoteFile(io.IOBase):
    def __init__(self, cmd, fd, is_text):
        self.cmd = cmd
        self.fd = fd
        self.is_text = is_text
    def ioctl(self, request, arg):
        if request == 4: # CLOSE
            self.close()
        return 0
    def close(self):
        if self.fd is None:
            return
        self.cmd.begin(CMD_CLOSE)
        self.cmd.wr_int32(self.fd)
        self.cmd.end()
        self.fd = None
    def read(self, n=-1):
        self.cmd.begin(CMD_READ)
        self.cmd.wr_int32(self.fd)
        self.cmd.wr_int32(n)
        data = self.cmd.rd_bytes()
        if self.is_text:
            data = str(data, 'utf8')
        self.cmd.end()
        return data
    def readinto(self, buf):
        self.cmd.begin(CMD_READ)
        self.cmd.wr_int32(self.fd)
        self.cmd.wr_int32(len(buf))
        n = self.cmd.rd_bytes_into(buf)
        self.cmd.end()
        return n
    def write(self, buf):
        self.cmd.begin(CMD_WRITE)
        self.cmd.wr_int32(self.fd)
        self.cmd.wr_bytes(buf)
        n = self.cmd.rd_int32()
        self.cmd.end()
        return n
    def seek(self, n):
        self.cmd.begin(CMD_SEEK)
        self.cmd.wr_int32(self.fd)
        self.cmd.wr_int32(n)
        n = self.cmd.rd_int32()
        self.cmd.end()
        return n

class RemoteFS:
    def mount(self, readonly, mkfs):
        self.cmd = RemoteCommand()
        self.readonly = readonly
    def chdir(self, path):
        if path.startswith('/'):
            self.path = path
        else:
            self.path += path
        if not self.path.endswith('/'):
            self.path += '/'
    def getcwd(self):
        return self.path
    def stat(self, path):
        self.cmd.begin(CMD_STAT)
        self.cmd.wr_str(self.path + path)
        res = self.cmd.rd_int32()
        if res < 0:
            raise OSError(-res)
        return tuple(self.cmd.rd_uint32() for _ in range(10))
    def ilistdir(self, path):
        self.cmd.begin(CMD_ILISTDIR_START)
        self.cmd.wr_str(self.path + path)
        self.cmd.end()
        def ilistdir_next():
            while True:
                self.cmd.begin(CMD_ILISTDIR_NEXT)
                name = self.cmd.rd_str()
                if name:
                    type = self.cmd.rd_uint32()
                    inode = self.cmd.rd_uint64()
                    self.cmd.end()
                    yield (name, type, inode)
                else:
                    self.cmd.end()
                    break
        return ilistdir_next()
    def open(self, path, mode):
        self.cmd.begin(CMD_OPEN)
        self.cmd.wr_str(self.path + path)
        self.cmd.wr_str(mode)
        fd = self.cmd.rd_int32()
        self.cmd.end()
        if fd < 0:
            raise OSError(-fd)
        return RemoteFile(self.cmd, fd, mode.find('b') == -1)

os.mount(RemoteFS(), '/remote')
os.chdir('/remote')
"""

class ConsolePosix:
    def __init__(self):
        self.infd = sys.stdin.fileno()
        self.infile = sys.stdin.buffer.raw
        self.outfile = sys.stdout.buffer.raw
        self.orig_attr = termios.tcgetattr(self.infd)
    def enter(self):
        # attr is: [iflag, oflag, cflag, lflag, ispeed, ospeed, cc]
        attr = termios.tcgetattr(self.infd)
        attr[0] &= ~(termios.BRKINT | termios.ICRNL | termios.INPCK | termios.ISTRIP | termios.IXON);
        attr[1] = 0;
        attr[2] = attr[2] & ~(termios.CSIZE | termios.PARENB) | termios.CS8;
        attr[3] = 0;
        attr[6][termios.VMIN] = 1
        attr[6][termios.VTIME] = 0
        termios.tcsetattr(self.infd, termios.TCSANOW, attr)

    def exit(self):
        termios.tcsetattr(self.infd, termios.TCSANOW, self.orig_attr)
    def readchar(self):
        res = select.select([self.infd], [], [], 0)
        if res[0]:
            return self.infile.read(1)
        else:
            return None
    def write(self, buf):
        self.outfile.write(buf)


class ConsoleWindows:
    def enter(self):
        pass

    def exit(self):
        pass

    def inWaiting(self):
        return 1 if msvcrt.kbhit() else 0
    
    def readchar(self):
        if msvcrt.kbhit():
            ch =  msvcrt.getch()
            while ch in b'\x00\xe0':  # arrow or function key prefix?
                if not msvcrt.kbhit():
                    return None
                ch = msvcrt.getch()  # second call returns the actual key code
            return ch

    def write(self, buf):
        buf = buf.decode() if isinstance(buf, bytes) else buf
        sys.stdout.write(buf)
        sys.stdout.flush()
        # for b in buf:
        #     if isinstance(b, bytes):
        #         msvcrt.putch(b)
        #     else:
        #         msvcrt.putwch(b)
                
if termios:
    Console = ConsolePosix
    VT_ENABLED = True
else:
    Console = ConsoleWindows

    # Windows VT mode ( >= win10 only)
    # https://bugs.python.org/msg291732
    import ctypes
    from ctypes import wintypes

    kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)

    ERROR_INVALID_PARAMETER = 0x0057
    ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004

    def _check_bool(result, func, args):
        if not result:
            raise ctypes.WinError(ctypes.get_last_error())
        return args

    LPDWORD = ctypes.POINTER(wintypes.DWORD)
    kernel32.GetConsoleMode.errcheck = _check_bool
    kernel32.GetConsoleMode.argtypes = (wintypes.HANDLE, LPDWORD)
    kernel32.SetConsoleMode.errcheck = _check_bool
    kernel32.SetConsoleMode.argtypes = (wintypes.HANDLE, wintypes.DWORD)

    def set_conout_mode(new_mode, mask=0xffffffff):
        # don't assume StandardOutput is a console.
        # open CONOUT$ instead
        fdout = os.open('CONOUT$', os.O_RDWR)
        try:
            hout = msvcrt.get_osfhandle(fdout)
            old_mode = wintypes.DWORD()
            kernel32.GetConsoleMode(hout, ctypes.byref(old_mode))
            mode = (new_mode & mask) | (old_mode.value & ~mask)
            kernel32.SetConsoleMode(hout, mode)
            return old_mode.value
        finally:
            os.close(fdout)

    # def enable_vt_mode():
    mode = mask = ENABLE_VIRTUAL_TERMINAL_PROCESSING
    try:
        set_conout_mode(mode, mask)
        VT_ENABLED=True
    except WindowsError as e:
        VT_ENABLED=False
        

class PyboardCommand:
    def __init__(self, fin, fout):
        self.fin = fin
        self.fout = fout
    def rd_uint32(self):
        return struct.unpack('<I', self.rd(4))[0]
    def wr_uint32(self, i):
        self.fout.write(struct.pack('<I', i))
    def rd_uint64(self):
        return struct.unpack('<Q', self.rd(8))[0]
    def wr_uint64(self, i):
        self.fout.write(struct.pack('<Q', i))
    def rd_int32(self):
        return struct.unpack('<i', self.fin.read(4))[0]
    def wr_int32(self, i):
        self.fout.write(struct.pack('<i', i))
    def rd_bytes(self):
        n = struct.unpack('<H', self.fin.read(2))[0]
        return self.fin.read(n)
    def wr_bytes(self, b):
        self.fout.write(struct.pack('<H', len(b)))
        self.fout.write(b)
    def rd_str(self):
        n = self.fin.read(1)[0]
        if n == 0:
            return ''
        else:
            return str(self.fin.read(n), 'utf8')
    def wr_str(self, s):
        b = bytes(s, 'utf8')
        l = len(b)
        assert l <= 255
        self.fout.write(bytearray([l]) + b)

root = './'
data_ilistdir = []
data_files = []

def do_stat(cmd):
    path = root + cmd.rd_str()
    try:
        stat = os.stat(path)
    except OSError as er:
        cmd.wr_int32(-abs(er.args[0]))
    else:
        cmd.wr_int32(0)
        assert len(stat) == 10
        for val in stat:
            cmd.wr_uint32(val) # TODO will all values always fit in 32 bits?

def do_ilistdir_start(cmd):
    global data_ilistdir
    path = root + cmd.rd_str()
    data_ilistdir = os.listdir(path)

def do_ilistdir_next(cmd):
    if data_ilistdir:
        entry = data_ilistdir.pop(0)
        stat = os.stat(entry)
        cmd.wr_str(entry)
        cmd.wr_uint32(stat.st_mode & 0xc000)
        cmd.wr_uint64(stat.st_ino)
    else:
        cmd.wr_str('')

def do_open(cmd):
    path = root + cmd.rd_str()
    mode = cmd.rd_str()
    try:
        f = open(path, mode)
    except OSError as er:
        cmd.wr_int32(-abs(er.args[0]))
    else:
        is_text = mode.find('b') == -1
        try:
            fd = data_files.index(None)
            data_files[fd] = (f, is_text)
        except ValueError:
            fd = len(data_files)
            data_files.append((f, is_text))
        cmd.wr_int32(fd)

def do_close(cmd):
    fd = cmd.rd_int32()
    data_files[fd][0].close()
    data_files[fd] = None

def do_read(cmd):
    fd = cmd.rd_int32()
    n = cmd.rd_int32()
    buf = data_files[fd][0].read(n)
    if data_files[fd][1]:
        buf = bytes(buf, 'utf8')
    cmd.wr_bytes(buf)

def do_seek(cmd):
    fd = cmd.rd_int32()
    n = cmd.rd_int32()
    data_files[fd][0].seek(n)
    cmd.wr_int32(n)

def do_write(cmd):
    fd = cmd.rd_int32()
    buf = cmd.rd_bytes()
    if data_files[fd][1]:
        buf = str(buf, 'utf8')
    n = data_files[fd][0].write(buf)
    cmd.wr_int32(n)

cmd_table = {
    CMD_STAT: do_stat,
    CMD_ILISTDIR_START: do_ilistdir_start,
    CMD_ILISTDIR_NEXT: do_ilistdir_next,
    CMD_OPEN: do_open,
    CMD_CLOSE: do_close,
    CMD_READ: do_read,
    CMD_WRITE: do_write,
    CMD_SEEK: do_seek,
}

def main_loop(console, dev, pyfile=None):
    # TODO add option to not restart pyboard, to continue a previous session
    try:
        pyb = pyboard.Pyboard(dev)
    except pyboard.PyboardError as er:
        print(er)
        return
    pyb.enter_raw_repl()
    pyb.exec_(fs_hook_code)
    cmd = PyboardCommand(pyb.serial, pyb.serial)

    console.write(bytes('Connected to MicroPython at %s\r\n' % dev, 'utf8'))
    console.write(bytes('Local directory %s is mounted at /remote\r\n' % root, 'utf8'))
    console.write(bytes('Use Ctrl-X to exit this shell\r\n', 'utf8'))

    if pyfiles:
        for pyfile in pyfiles:
            script = Path(pyfile)
            if not script.exists():
                console.write(bytes('\r\nERROR: Provided script not found!\r\n', 'utf8'))
            else:
                pyb.exec_(script.read_bytes())

    pyb.exit_raw_repl()

    while True:
        if isinstance(console, ConsolePosix):
            select.select([console.infd, pyb.serial.fd], [], []) # TODO pyb.serial might not have fd
        else:
            while not (console.inWaiting() or pyb.serial.inWaiting()):
                time.sleep(0.1)
        c = console.readchar()
        if c:
            if c == b'\x18': # ctrl-X, quit
                break
            elif c == b'\x04': # ctrl-D
                # do a soft reset and relead the filesystem hook
                #console.write(b'\r\n[soft reset and execute filesystem hook]\r\n')
                pyb.serial.write(b'\x04')
                console.write(pyb.serial.read(1))
                n = pyb.serial.inWaiting()
                while n > 0:
                    buf = pyb.serial.read(n)
                    console.write(buf)
                    time.sleep(0.1)
                    n = pyb.serial.inWaiting()
                pyb.serial.write(b'\x01')
                pyb.exec_(fs_hook_code)
                pyb.serial.write(b'\x02')
                time.sleep(0.1)
                pyb.serial.read(1)
                n = pyb.serial.inWaiting()
                while n > 0:
                    buf = pyb.serial.read(n)
                    time.sleep(0.1)
                    n = pyb.serial.inWaiting()
            else:
                # pass character through to the pyboard
                pyb.serial.write(c)

        if pyb.serial.inWaiting() > 0:
            c = pyb.serial.read(1)
            if c == b'\x18':
                # a special command
                c = pyb.serial.read(1)[0]
                cmd_table[c](cmd)

            elif not VT_ENABLED and c == b'\x1b':
                # ESC code, ignore these on windows
                esctype = pyb.serial.read(1)
                if esctype == b'[':  # CSI
                    while not (0x40 < pyb.serial.read(1)[0] < 0x7E):
                        # Looking for "final byte" of escape sequence
                        pass
            else:
                # pass character through to the console
                console.write(c)

def main():
    # get serial device
    if len(sys.argv) == 1:
        dev = None
        for d in ('/dev/ttyACM0', '/dev/ttyACM1', '/dev/ttyUSB0', '/dev/ttyUSB1'):
            try:
                os.stat(d)
            except OSError:
                continue
            dev = d
            break
        if dev is None:
            print('no device found')
            sys.exit(1)
    else:
        dev = sys.argv[1]
        shortcuts = {'a0': '/dev/ttyACM0', 'a1': '/dev/ttyACM1', 'u0': '/dev/ttyUSB0', 'u1': '/dev/ttyUSB1'}
        dev = shortcuts.get(dev, dev)
    
    pyfile = sys.argv[2] if len(sys.argv) >= 3 else None

    console = Console()
    console.enter()
    try:
        main_loop(console, dev, pyfile)
    finally:
        console.exit()

if __name__ == '__main__':
    main()
