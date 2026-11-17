import codecs
import io
import os
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
    _USE_RAW_OUTPUT = None
    _CONSOLE_CP_UTF8_SET = False

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
        self._use_raw_output = ConsoleWindows._USE_RAW_OUTPUT
        if self._use_raw_output is None:
            self._use_raw_output = ConsoleWindows._detect_modern_console(sys.stdout)
        if self._use_raw_output:
            # Modern console: write raw UTF-8 bytes like POSIX
            self.outfile = sys.stdout.buffer
            if hasattr(self.outfile, "raw"):
                self.outfile = self.outfile.raw
            # Ensure console interprets output as UTF-8
            ConsoleWindows._ensure_console_output_utf8()
        else:
            # Legacy console: use incremental decoder to handle split UTF-8 sequences
            self._decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")

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
        if self._use_raw_output:
            # Modern console: write raw UTF-8 bytes directly (like POSIX)
            if isinstance(buf, str):
                buf = buf.encode("utf-8")
            self.outfile.write(buf)
            self.outfile.flush()
        else:
            # Legacy console: use incremental decoder to handle split UTF-8 sequences
            if isinstance(buf, bytes):
                buf = self._decoder.decode(buf)
            if buf:
                sys.stdout.write(buf)
                sys.stdout.flush()

    @staticmethod
    def enable_vt_mode():
        # Windows VT mode (>= win10 only)
        # https://bugs.python.org/msg291732
        import ctypes
        from ctypes import wintypes

        kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)

        ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004

        def _check_bool(result, func, args):
            if not result:
                raise ctypes.WinError(ctypes.get_last_error())
            return args

        lpdword = ctypes.POINTER(wintypes.DWORD)
        kernel32.GetConsoleMode.errcheck = _check_bool
        kernel32.GetConsoleMode.argtypes = (wintypes.HANDLE, lpdword)
        kernel32.SetConsoleMode.errcheck = _check_bool
        kernel32.SetConsoleMode.argtypes = (wintypes.HANDLE, wintypes.DWORD)

        def set_conout_mode(new_mode, mask=0xFFFFFFFF):
            # Don't assume StandardOutput is a console.
            # Open CONOUT$ instead.
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

        mode = mask = ENABLE_VIRTUAL_TERMINAL_PROCESSING
        try:
            set_conout_mode(mode, mask)
            return True
        except WindowsError:
            return False

    @classmethod
    def configure_unicode_output(cls):
        """Configure stdout/stderr for Unicode support on Windows legacy consoles."""
        if sys.platform != "win32":
            return

        # Cache the modern-console detection result for ConsoleWindows.
        if cls._USE_RAW_OUTPUT is None:
            cls._USE_RAW_OUTPUT = cls._detect_modern_console(sys.stdout)

        # Set console output code page to UTF-8 for raw byte output.
        cls._ensure_console_output_utf8()

        # Full Unicode encodings that don't need fixing.
        unicode_encodings = {"utf-8", "utf-16", "utf-16-le", "utf-16-be", "utf-32"}

        for stream_name in ("stdout", "stderr"):
            stream = getattr(sys, stream_name, None)
            if stream is None or not hasattr(stream, "encoding"):
                continue

            encoding = (stream.encoding or "").lower().replace("_", "-")

            # Skip if already a full Unicode encoding.
            if encoding in unicode_encodings:
                continue

            # Reconfigure with UTF-8 and error handling for limited encodings.
            if hasattr(stream, "buffer"):
                wrapped = io.TextIOWrapper(
                    stream.buffer,
                    encoding="utf-8",
                    errors="backslashreplace",
                )
                setattr(sys, stream_name, wrapped)

    @staticmethod
    def _detect_modern_console(stream):
        """Detect if running in a modern Windows console that supports raw UTF-8."""
        # Windows Terminal
        if os.environ.get("WT_SESSION"):
            return True
        # VS Code integrated terminal
        if os.environ.get("TERM_PROGRAM") == "vscode":
            return True
        # ConEmu/Cmder
        if os.environ.get("ConEmuANSI") == "ON":
            return True

        encoding = getattr(stream, "encoding", None)
        if encoding:
            encoding = encoding.lower().replace("_", "-")
            if encoding in ("utf-8", "utf8"):
                return True
        return False

    @classmethod
    def _ensure_console_output_utf8(cls):
        if cls._CONSOLE_CP_UTF8_SET:
            return
        try:
            import ctypes

            ctypes.windll.kernel32.SetConsoleOutputCP(65001)
        except Exception:
            pass
        cls._CONSOLE_CP_UTF8_SET = True


if termios:
    Console = ConsolePosix
    VT_ENABLED = True
else:
    Console = ConsoleWindows
    VT_ENABLED = ConsoleWindows.enable_vt_mode()
