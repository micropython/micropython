import sys, time

try:
    import select, termios
except ImportError:
    termios = None
    select = None
    import msvcrt, signal


class ConsolePosix:
    def __init__(self):
        self.infd = sys.stdin.fileno()
        self.infile = sys.stdin.buffer
        self.outfile = sys.stdout.buffer
        if hasattr(self.infile, "raw"):
            self.infile = self.infile.raw
        if hasattr(self.outfile, "raw"):
            self.outfile = self.outfile.raw

        self.orig_attr = termios.tcgetattr(self.infd)

    def enter(self):
        # attr is: [iflag, oflag, cflag, lflag, ispeed, ospeed, cc]
        attr = termios.tcgetattr(self.infd)
        attr[0] &= ~(
            termios.BRKINT | termios.ICRNL | termios.INPCK | termios.ISTRIP | termios.IXON
        )
        attr[1] = 0
        attr[2] = attr[2] & ~(termios.CSIZE | termios.PARENB) | termios.CS8
        attr[3] = 0
        attr[6][termios.VMIN] = 1
        attr[6][termios.VTIME] = 0
        termios.tcsetattr(self.infd, termios.TCSANOW, attr)

    def exit(self):
        termios.tcsetattr(self.infd, termios.TCSANOW, self.orig_attr)

    def waitchar(self, pyb_serial):
        # TODO pyb_serial might not have fd
        select.select([self.infd, pyb_serial.fd], [], [])

    def readchar(self):
        res = select.select([self.infd], [], [], 0)
        if res[0]:
            return self.infile.read(1)
        else:
            return None

    def write(self, buf):
        self.outfile.write(buf)


class ConsoleWindows:
    KEY_MAP = {
        b"H": b"A",  # UP
        b"P": b"B",  # DOWN
        b"M": b"C",  # RIGHT
        b"K": b"D",  # LEFT
        b"G": b"H",  # POS1
        b"O": b"F",  # END
        b"Q": b"6~",  # PGDN
        b"I": b"5~",  # PGUP
        b"s": b"1;5D",  # CTRL-LEFT,
        b"t": b"1;5C",  # CTRL-RIGHT,
        b"\x8d": b"1;5A",  #  CTRL-UP,
        b"\x91": b"1;5B",  # CTRL-DOWN,
        b"w": b"1;5H",  # CTRL-POS1
        b"u": b"1;5F",  # CTRL-END
        b"\x98": b"1;3A",  #  ALT-UP,
        b"\xa0": b"1;3B",  # ALT-DOWN,
        b"\x9d": b"1;3C",  #  ALT-RIGHT,
        b"\x9b": b"1;3D",  # ALT-LEFT,
        b"\x97": b"1;3H",  #  ALT-POS1,
        b"\x9f": b"1;3F",  # ALT-END,
        b"S": b"3~",  # DEL,
        b"\x93": b"3;5~",  # CTRL-DEL
        b"R": b"2~",  # INS
        b"\x92": b"2;5~",  # CTRL-INS
        b"\x94": b"Z",  # Ctrl-Tab = BACKTAB,
    }

    def __init__(self):
        self.ctrl_c = 0

    def _sigint_handler(self, signo, frame):
        self.ctrl_c += 1

    def enter(self):
        signal.signal(signal.SIGINT, self._sigint_handler)

    def exit(self):
        signal.signal(signal.SIGINT, signal.SIG_DFL)

    def inWaiting(self):
        return 1 if self.ctrl_c or msvcrt.kbhit() else 0

    def waitchar(self, pyb_serial):
        while not (self.inWaiting() or pyb_serial.inWaiting()):
            time.sleep(0.01)

    def readchar(self):
        if self.ctrl_c:
            self.ctrl_c -= 1
            return b"\x03"
        if msvcrt.kbhit():
            ch = msvcrt.getch()
            while ch in b"\x00\xe0":  # arrow or function key prefix?
                if not msvcrt.kbhit():
                    return None
                ch = msvcrt.getch()  # second call returns the actual key code
                try:
                    ch = b"\x1b[" + self.KEY_MAP[ch]
                except KeyError:
                    return None
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
    import ctypes, os
    from ctypes import wintypes

    kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)

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

    def set_conout_mode(new_mode, mask=0xFFFFFFFF):
        # don't assume StandardOutput is a console.
        # open CONOUT$ instead
        fdout = os.open("CONOUT$", os.O_RDWR)
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
        VT_ENABLED = True
    except WindowsError:
        VT_ENABLED = False
