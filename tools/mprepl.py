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
import termios
import pyboard

CMD_STAT = 1
CMD_LISTDIR_START = 2
CMD_LISTDIR_NEXT = 3
CMD_OPEN = 4
CMD_CLOSE = 5
CMD_READ = 6
CMD_WRITE = 7

fs_hook_code = """\
import os, io, select, ustruct as struct, micropython
CMD_STAT = 1
CMD_LISTDIR_START = 2
CMD_LISTDIR_NEXT = 3
CMD_OPEN = 4
CMD_CLOSE = 5
CMD_READ = 6
CMD_WRITE = 7
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
    def rd_int32(self):
        return struct.unpack('<i', self.rd(4))[0]
    def wr_int32(self, i):
        self.fout.write(struct.pack('<i', i))
    def rd_bytes(self):
        n = struct.unpack('<H', self.rd(2))[0]
        return self.rd(n)
    def rd_bytes_into(self, buf):
        n = struct.unpack('<H', self.rd(2))[0]
        return self.rdinto(buf)
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
    def close(self):
        if self.fd is None:
            return
        self.cmd.begin(CMD_CLOSE)
        self.cmd.wr_int32(self.fd)
        self.cmd.end()
        self.fd = None
    def read(self, n):
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
    def listdir(self, path):
        l = []
        self.cmd.begin(CMD_LISTDIR_START)
        self.cmd.wr_str(self.path + path)
        while True:
            self.cmd.begin(CMD_LISTDIR_NEXT)
            entry = self.cmd.rd_str()
            if entry:
                l.append(entry)
            else:
                break
        self.cmd.end()
        return l
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

class Console:
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

class PyboardCommand:
    def __init__(self, fin, fout):
        self.fin = fin
        self.fout = fout
    def rd_uint32(self):
        return struct.unpack('<I', self.rd(4))[0]
    def wr_uint32(self, i):
        self.fout.write(struct.pack('<I', i))
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
data_listdir = []
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

def do_listdir_start(cmd):
    global data_listdir
    path = root + cmd.rd_str()
    data_listdir = os.listdir(path)

def do_listdir_next(cmd):
    if data_listdir:
        entry = data_listdir.pop(0)
        cmd.wr_str(entry)
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

def do_write(cmd):
    fd = cmd.rd_int32()
    buf = cmd.rd_bytes()
    if data_files[fd][1]:
        buf = str(buf, 'utf8')
    n = data_files[fd][0].write(buf)
    cmd.wr_int32(n)

cmd_table = {
    CMD_STAT: do_stat,
    CMD_LISTDIR_START: do_listdir_start,
    CMD_LISTDIR_NEXT: do_listdir_next,
    CMD_OPEN: do_open,
    CMD_CLOSE: do_close,
    CMD_READ: do_read,
    CMD_WRITE: do_write,
}

def main_loop(console, dev):
    # TODO add option to not restart pyboard, to continue a previous session
    pyb = pyboard.Pyboard(dev)
    pyb.enter_raw_repl()
    pyb.exec_(fs_hook_code)
    pyb.exit_raw_repl()
    cmd = PyboardCommand(pyb.serial, pyb.serial)

    console.write(bytes('Connected to MicroPython at %s\r\n' % dev, 'utf8'))
    console.write(bytes('Local directory %s is mounted at /remote\r\n' % root, 'utf8'))
    console.write(bytes('Use Ctrl-X to exit this shell\r\n', 'utf8'))

    while True:
        select.select([console.infd, pyb.serial.fd], [], []) # TODO pyb.serial might not have fd
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
            else:
                # pass character through to the console
                console.write(c)

def main():
    if len(sys.argv) == 1:
        dev = '/dev/ttyACM0'
    else:
        dev = sys.argv[1]

    console = Console()
    console.enter()
    try:
        main_loop(console, dev)
    finally:
        console.exit()

if __name__ == '__main__':
    main()
