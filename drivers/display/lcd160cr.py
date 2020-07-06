# Driver for official MicroPython LCD160CR display
# MIT license; Copyright (c) 2017 Damien P. George

from micropython import const
from utime import sleep_ms
from ustruct import calcsize, pack_into
import uerrno, machine

# for set_orient
PORTRAIT = const(0)
LANDSCAPE = const(1)
PORTRAIT_UPSIDEDOWN = const(2)
LANDSCAPE_UPSIDEDOWN = const(3)

# for set_startup_deco; can be or'd
STARTUP_DECO_NONE = const(0)
STARTUP_DECO_MLOGO = const(1)
STARTUP_DECO_INFO = const(2)

_uart_baud_table = {
    2400: 0,
    4800: 1,
    9600: 2,
    19200: 3,
    38400: 4,
    57600: 5,
    115200: 6,
    230400: 7,
    460800: 8,
}


class LCD160CR:
    def __init__(self, connect=None, *, pwr=None, i2c=None, spi=None, i2c_addr=98):
        if connect in ("X", "Y", "XY", "YX"):
            i = connect[-1]
            j = connect[0]
            y = j + "4"
        elif connect == "C":
            i = 2
            j = 2
            y = "A7"
        else:
            if pwr is None or i2c is None or spi is None:
                raise ValueError('must specify valid "connect" or all of "pwr", "i2c" and "spi"')

        if pwr is None:
            pwr = machine.Pin(y, machine.Pin.OUT)
        if i2c is None:
            i2c = machine.I2C(i, freq=1000000)
        if spi is None:
            spi = machine.SPI(j, baudrate=13500000, polarity=0, phase=0)

        if not pwr.value():
            pwr(1)
            sleep_ms(10)
        # else:
        # alread have power
        # lets be optimistic...

        # set connections
        self.pwr = pwr
        self.i2c = i2c
        self.spi = spi
        self.i2c_addr = i2c_addr

        # create temp buffers and memoryviews
        self.buf16 = bytearray(16)
        self.buf19 = bytearray(19)
        self.buf = [None] * 10
        for i in range(1, 10):
            self.buf[i] = memoryview(self.buf16)[0:i]
        self.buf1 = self.buf[1]
        self.array4 = [0, 0, 0, 0]

        # set default orientation and window
        self.set_orient(PORTRAIT)
        self._fcmd2b("<BBBBBB", 0x76, 0, 0, self.w, self.h)  # viewport 'v'
        self._fcmd2b("<BBBBBB", 0x79, 0, 0, self.w, self.h)  # window 'y'

    def _send(self, cmd):
        i = self.i2c.writeto(self.i2c_addr, cmd)
        if i == len(cmd):
            return
        cmd = memoryview(cmd)
        n = len(cmd)
        while True:
            i += self.i2c.writeto(self.i2c_addr, cmd[i:])
            if i == n:
                return
            sleep_ms(10)

    def _fcmd2(self, fmt, a0, a1=0, a2=0):
        buf = self.buf[calcsize(fmt)]
        pack_into(fmt, buf, 0, 2, a0, a1, a2)
        self._send(buf)

    def _fcmd2b(self, fmt, a0, a1, a2, a3, a4=0):
        buf = self.buf[calcsize(fmt)]
        pack_into(fmt, buf, 0, 2, a0, a1, a2, a3, a4)
        self._send(buf)

    def _waitfor(self, n, buf):
        t = 5000
        while t:
            self.i2c.readfrom_into(self.i2c_addr, self.buf1)
            if self.buf1[0] >= n:
                self.i2c.readfrom_into(self.i2c_addr, buf)
                return
            t -= 1
            sleep_ms(1)
        raise OSError(uerrno.ETIMEDOUT)

    def oflush(self, n=255):
        t = 5000
        while t:
            self.i2c.readfrom_into(self.i2c_addr + 1, self.buf1)
            r = self.buf1[0]
            if r >= n:
                return
            t -= 1
            machine.idle()
        raise OSError(uerrno.ETIMEDOUT)

    def iflush(self):
        t = 5000
        while t:
            self.i2c.readfrom_into(self.i2c_addr, self.buf16)
            if self.buf16[0] == 0:
                return
            t -= 1
            sleep_ms(1)
        raise OSError(uerrno.ETIMEDOUT)

    #### MISC METHODS ####

    @staticmethod
    def rgb(r, g, b):
        return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3)

    @staticmethod
    def clip_line(c, w, h):
        while True:
            ca = ce = 0
            if c[1] < 0:
                ca |= 8
            elif c[1] > h:
                ca |= 4
            if c[0] < 0:
                ca |= 1
            elif c[0] > w:
                ca |= 2
            if c[3] < 0:
                ce |= 8
            elif c[3] > h:
                ce |= 4
            if c[2] < 0:
                ce |= 1
            elif c[2] > w:
                ce |= 2
            if ca & ce:
                return False
            elif ca | ce:
                ca |= ce
                if ca & 1:
                    if c[2] < c[0]:
                        c[0], c[2] = c[2], c[0]
                        c[1], c[3] = c[3], c[1]
                    c[1] += ((-c[0]) * (c[3] - c[1])) // (c[2] - c[0])
                    c[0] = 0
                elif ca & 2:
                    if c[2] < c[0]:
                        c[0], c[2] = c[2], c[0]
                        c[1], c[3] = c[3], c[1]
                    c[3] += ((w - 1 - c[2]) * (c[3] - c[1])) // (c[2] - c[0])
                    c[2] = w - 1
                elif ca & 4:
                    if c[0] == c[2]:
                        if c[1] >= h:
                            c[1] = h - 1
                        if c[3] >= h:
                            c[3] = h - 1
                    else:
                        if c[3] < c[1]:
                            c[0], c[2] = c[2], c[0]
                            c[1], c[3] = c[3], c[1]
                        c[2] += ((h - 1 - c[3]) * (c[2] - c[0])) // (c[3] - c[1])
                        c[3] = h - 1
                else:
                    if c[0] == c[2]:
                        if c[1] < 0:
                            c[1] = 0
                        if c[3] < 0:
                            c[3] = 0
                    else:
                        if c[3] < c[1]:
                            c[0], c[2] = c[2], c[0]
                            c[1], c[3] = c[3], c[1]
                        c[0] += ((-c[1]) * (c[2] - c[0])) // (c[3] - c[1])
                        c[1] = 0
            else:
                return True

    #### SETUP COMMANDS ####

    def set_power(self, on):
        self.pwr(on)
        sleep_ms(15)

    def set_orient(self, orient):
        self._fcmd2("<BBB", 0x14, (orient & 3) + 4)
        # update width and height variables
        self.iflush()
        self._send(b"\x02g0")
        self._waitfor(4, self.buf[5])
        self.w = self.buf[5][1]
        self.h = self.buf[5][2]

    def set_brightness(self, value):
        self._fcmd2("<BBB", 0x16, value)

    def set_i2c_addr(self, addr):
        # 0x0e set i2c addr
        if addr & 3:
            raise ValueError("must specify mod 4 aligned address")
        self._fcmd2("<BBW", 0x0E, 0x433249 | (addr << 24))

    def set_uart_baudrate(self, baudrate):
        try:
            baudrate = _uart_baud_table[baudrate]
        except KeyError:
            raise ValueError("invalid baudrate")
        self._fcmd2("<BBB", 0x18, baudrate)

    def set_startup_deco(self, value):
        self._fcmd2("<BBB", 0x19, value)

    def save_to_flash(self):
        self._send(b"\x02fn")

    #### PIXEL ACCESS ####

    def set_pixel(self, x, y, c):
        self._fcmd2b("<BBBBH", 0x41, x, y, c)

    def get_pixel(self, x, y):
        self._fcmd2("<BBBB", 0x61, x, y)
        t = 1000
        while t:
            self.i2c.readfrom_into(self.i2c_addr, self.buf1)
            if self.buf1[0] >= 2:
                self.i2c.readfrom_into(self.i2c_addr, self.buf[3])
                return self.buf[3][1] | self.buf[3][2] << 8
            t -= 1
            sleep_ms(1)
        raise OSError(uerrno.ETIMEDOUT)

    def get_line(self, x, y, buf):
        l = len(buf) // 2
        self._fcmd2b("<BBBBB", 0x10, l, x, y)
        l *= 2
        t = 1000
        while t:
            self.i2c.readfrom_into(self.i2c_addr, self.buf1)
            if self.buf1[0] >= l:
                self.i2c.readfrom_into(self.i2c_addr, buf)
                return
            t -= 1
            sleep_ms(1)
        raise OSError(uerrno.ETIMEDOUT)

    def screen_dump(self, buf, x=0, y=0, w=None, h=None):
        if w is None:
            w = self.w - x
        if h is None:
            h = self.h - y
        if w <= 127:
            line = bytearray(2 * w + 1)
            line2 = None
        else:
            # split line if more than 254 bytes needed
            buflen = (w + 1) // 2
            line = bytearray(2 * buflen + 1)
            line2 = memoryview(line)[: 2 * (w - buflen) + 1]
        for i in range(min(len(buf) // (2 * w), h)):
            ix = i * w * 2
            self.get_line(x, y + i, line)
            buf[ix : ix + len(line) - 1] = memoryview(line)[1:]
            ix += len(line) - 1
            if line2:
                self.get_line(x + buflen, y + i, line2)
                buf[ix : ix + len(line2) - 1] = memoryview(line2)[1:]
                ix += len(line2) - 1

    def screen_load(self, buf):
        l = self.w * self.h * 2 + 2
        self._fcmd2b("<BBHBBB", 0x70, l, 16, self.w, self.h)
        n = 0
        ar = memoryview(buf)
        while n < len(buf):
            if len(buf) - n >= 0x200:
                self._send(ar[n : n + 0x200])
                n += 0x200
            else:
                self._send(ar[n:])
                while n < self.w * self.h * 2:
                    self._send(b"\x00")
                    n += 1

    #### TEXT COMMANDS ####

    def set_pos(self, x, y):
        self._fcmd2("<BBBB", 0x58, x, y)

    def set_text_color(self, fg, bg):
        self._fcmd2("<BBHH", 0x63, fg, bg)

    def set_font(self, font, scale=0, bold=0, trans=0, scroll=0):
        self._fcmd2(
            "<BBBB",
            0x46,
            (scroll << 7) | (trans << 6) | ((font & 3) << 4) | (bold & 0xF),
            scale & 0xFF,
        )

    def write(self, s):
        # TODO: eventually check for room in LCD input queue
        self._send(s)

    #### PRIMITIVE DRAWING COMMANDS ####

    def set_pen(self, line, fill):
        self._fcmd2("<BBHH", 0x50, line, fill)

    def erase(self):
        self._send(b"\x02\x45")

    def dot(self, x, y):
        if 0 <= x < self.w and 0 <= y < self.h:
            self._fcmd2("<BBBB", 0x4B, x, y)

    def rect(self, x, y, w, h, cmd=0x72):
        if x + w <= 0 or y + h <= 0 or x >= self.w or y >= self.h:
            return
        elif x < 0 or y < 0:
            left = top = True
            if x < 0:
                left = False
                w += x
                x = 0
            if y < 0:
                top = False
                h += y
                y = 0
            if cmd == 0x51 or cmd == 0x72:
                # draw interior
                self._fcmd2b("<BBBBBB", 0x51, x, y, min(w, 255), min(h, 255))
            if cmd == 0x57 or cmd == 0x72:
                # draw outline
                if left:
                    self._fcmd2b("<BBBBBB", 0x57, x, y, 1, min(h, 255))
                if top:
                    self._fcmd2b("<BBBBBB", 0x57, x, y, min(w, 255), 1)
                if x + w < self.w:
                    self._fcmd2b("<BBBBBB", 0x57, x + w, y, 1, min(h, 255))
                if y + h < self.h:
                    self._fcmd2b("<BBBBBB", 0x57, x, y + h, min(w, 255), 1)
        else:
            self._fcmd2b("<BBBBBB", cmd, x, y, min(w, 255), min(h, 255))

    def rect_outline(self, x, y, w, h):
        self.rect(x, y, w, h, 0x57)

    def rect_interior(self, x, y, w, h):
        self.rect(x, y, w, h, 0x51)

    def line(self, x1, y1, x2, y2):
        ar4 = self.array4
        ar4[0] = x1
        ar4[1] = y1
        ar4[2] = x2
        ar4[3] = y2
        if self.clip_line(ar4, self.w, self.h):
            self._fcmd2b("<BBBBBB", 0x4C, ar4[0], ar4[1], ar4[2], ar4[3])

    def dot_no_clip(self, x, y):
        self._fcmd2("<BBBB", 0x4B, x, y)

    def rect_no_clip(self, x, y, w, h):
        self._fcmd2b("<BBBBBB", 0x72, x, y, w, h)

    def rect_outline_no_clip(self, x, y, w, h):
        self._fcmd2b("<BBBBBB", 0x57, x, y, w, h)

    def rect_interior_no_clip(self, x, y, w, h):
        self._fcmd2b("<BBBBBB", 0x51, x, y, w, h)

    def line_no_clip(self, x1, y1, x2, y2):
        self._fcmd2b("<BBBBBB", 0x4C, x1, y1, x2, y2)

    def poly_dot(self, data):
        if len(data) & 1:
            raise ValueError("must specify even number of bytes")
        self._fcmd2("<BBB", 0x71, len(data) // 2)
        self._send(data)

    def poly_line(self, data):
        if len(data) & 1:
            raise ValueError("must specify even number of bytes")
        self._fcmd2("<BBB", 0x78, len(data) // 2)
        self._send(data)

    #### TOUCH COMMANDS ####

    def touch_config(self, calib=False, save=False, irq=None):
        self._fcmd2("<BBBB", 0x7A, (irq is not None) << 2 | save << 1 | calib, bool(irq) << 7)

    def is_touched(self):
        self._send(b"\x02T")
        b = self.buf[4]
        self._waitfor(3, b)
        return b[1] >> 7 != 0

    def get_touch(self):
        self._send(b"\x02T")  # implicit LCD output flush
        b = self.buf[4]
        self._waitfor(3, b)
        return b[1] >> 7, b[2], b[3]

    #### ADVANCED COMMANDS ####

    def set_spi_win(self, x, y, w, h):
        pack_into(
            "<BBBHHHHHHHH", self.buf19, 0, 2, 0x55, 10, x, y, x + w - 1, y + h - 1, 0, 0, 0, 0xFFFF
        )
        self._send(self.buf19)

    def fast_spi(self, flush=True):
        self._send(b"\x02\x12")
        if flush:
            self.oflush()
        return self.spi

    def show_framebuf(self, buf):
        self.fast_spi().write(buf)

    def set_scroll(self, on):
        self._fcmd2("<BBB", 0x15, on)

    def set_scroll_win(self, win, x=-1, y=0, w=0, h=0, vec=0, pat=0, fill=0x07E0, color=0):
        pack_into("<BBBHHHHHHHH", self.buf19, 0, 2, 0x55, win, x, y, w, h, vec, pat, fill, color)
        self._send(self.buf19)

    def set_scroll_win_param(self, win, param, value):
        self._fcmd2b("<BBBBH", 0x75, win, param, value)

    def set_scroll_buf(self, s):
        l = len(s)
        if l > 32:
            raise ValueError("length must be 32 or less")
        self._fcmd2("<BBB", 0x11, l)
        self._send(s)

    def jpeg_start(self, l):
        if l > 0xFFFF:
            raise ValueError("length must be 65535 or less")
        self.oflush()
        self._fcmd2("<BBH", 0x6A, l)

    def jpeg_data(self, buf):
        self._send(buf)

    def jpeg(self, buf):
        self.jpeg_start(len(buf))
        self.jpeg_data(buf)

    def feed_wdt(self):
        self._send(b"\x02\x17")

    def reset(self):
        self._send(b"\x02Y\xef\xbe\xad\xde")
        sleep_ms(15)
