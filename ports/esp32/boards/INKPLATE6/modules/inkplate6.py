# Copyright © 2020 by Thorsten von Eicken.
import time
import micropython
import framebuf
import os
from machine import ADC, I2C, Pin, SDCard
from uarray import array
from mcp23017 import MCP23017
from micropython import const
from shapes import Shapes

from gfx import GFX
from gfx_standard_font_01 import text_dict as std_font
# ===== Constants that change between the Inkplate 6 and 10

# Raw display constants for Inkplate 6
D_ROWS = const(600)
D_COLS = const(800)

# Waveforms for 2 bits per pixel grey-scale.
# Order of 4 values in each tuple: blk, dk-grey, light-grey, white
# Meaning of values: 0=dischg, 1=black, 2=white, 3=skip
# Uses "colors" 0 (black), 3, 5, and 7 (white) from 3-bit waveforms below

# add discharge to waveforms to try to fix them
WAVE_2B = (  # original mpy driver for Ink 6, differs from arduino driver below
    (0, 0, 0, 0),
    (0, 0, 0, 0),
    (0, 1, 1, 0),
    (0, 1, 1, 0),
    (1, 2, 1, 0),
    (1, 1, 2, 0),
    (1, 2, 2, 2),
)
# Ink6 WAVEFORM3BIT from arduino driver
# {{0,1,1,0,0,1,1,0},{0,1,2,1,1,2,1,0},{1,1,1,2,2,1,0,0},{0,0,0,1,1,1,2,0},
#  {2,1,1,1,2,1,2,0},{2,2,1,1,2,1,2,0},{1,1,1,2,1,2,2,0},{0,0,0,0,0,0,2,0}};

TPS65186_addr = const(0x48)  # I2C address

# ESP32 GPIO set and clear registers to twiddle 32 gpio bits at once
# from esp-idf:
# define DR_REG_GPIO_BASE           0x3ff44000
# define GPIO_OUT_W1TS_REG          (DR_REG_GPIO_BASE + 0x0008)
# define GPIO_OUT_W1TC_REG          (DR_REG_GPIO_BASE + 0x000c)
ESP32_GPIO = const(0x3FF44000)  # ESP32 GPIO base
# register offsets from ESP32_GPIO
W1TS0 = const(2)  # offset for "write one to set" register for gpios 0..31
W1TC0 = const(3)  # offset for "write one to clear" register for gpios 0..31
W1TS1 = const(5)  # offset for "write one to set" register for gpios 32..39
W1TC1 = const(6)  # offset for "write one to clear" register for gpios 32..39
# bit masks in W1TS/W1TC registers
EPD_DATA = const(0x0E8C0030)  # EPD_D0..EPD_D7
EPD_CL = const(0x00000001)  # in W1Tx0
EPD_LE = const(0x00000004)  # in W1Tx0
EPD_CKV = const(0x00000001)  # in W1Tx1
EPD_SPH = const(0x00000002)  # in W1Tx1

# Inkplate provides access to the pins of the Inkplate 6 as well as to low-level display
# functions.


class _Inkplate:
    @classmethod
    def init(cls, i2c):
        cls._i2c = i2c
        cls._mcp23017 = MCP23017(i2c)
        # Display control lines
        cls.EPD_CL = Pin(0, Pin.OUT, value=0)
        cls.EPD_LE = Pin(2, Pin.OUT, value=0)
        cls.EPD_CKV = Pin(32, Pin.OUT, value=0)
        cls.EPD_SPH = Pin(33, Pin.OUT, value=1)
        cls.EPD_OE = cls._mcp23017.pin(0, Pin.OUT, value=0)
        cls.EPD_GMODE = cls._mcp23017.pin(1, Pin.OUT, value=0)
        cls.EPD_SPV = cls._mcp23017.pin(2, Pin.OUT, value=1)
        # Display data lines - we only use the Pin class to init the pins
        Pin(4, Pin.OUT)
        Pin(5, Pin.OUT)
        Pin(18, Pin.OUT)
        Pin(19, Pin.OUT)
        Pin(23, Pin.OUT)
        Pin(25, Pin.OUT)
        Pin(26, Pin.OUT)
        Pin(27, Pin.OUT)
        # TPS65186 power regulator control
        cls.TPS_WAKEUP = cls._mcp23017.pin(3, Pin.OUT, value=0)
        cls.TPS_PWRUP = cls._mcp23017.pin(4, Pin.OUT, value=0)
        cls.TPS_VCOM = cls._mcp23017.pin(5, Pin.OUT, value=0)
        cls.TPS_INT = cls._mcp23017.pin(6, Pin.IN)
        cls.TPS_PWR_GOOD = cls._mcp23017.pin(7, Pin.IN)
        # Misc
        cls.GPIO0_PUP = cls._mcp23017.pin(8, Pin.OUT, value=0)
        cls.VBAT_EN = cls._mcp23017.pin(9, Pin.OUT, value=1)
        cls.VBAT = ADC(Pin(35))
        cls.VBAT.atten(ADC.ATTN_11DB)
        cls.VBAT.width(ADC.WIDTH_12BIT)
        # Touch sensors
        cls.TOUCH1 = cls._mcp23017.pin(10, Pin.IN)
        cls.TOUCH2 = cls._mcp23017.pin(11, Pin.IN)
        cls.TOUCH3 = cls._mcp23017.pin(12, Pin.IN)

        cls._on = False  # whether panel is powered on or not

        if len(_Inkplate.byte2gpio) == 0:
            _Inkplate.gen_byte2gpio()

    @classmethod
    def begin(self):
        _Inkplate.init(I2C(0, scl=Pin(22), sda=Pin(21)))

        self.ipg = InkplateGS2()
        self.ipm = InkplateMono()
        self.ipp = InkplatePartial(self.ipm)

        self.GFX = GFX(
            D_COLS,
            D_ROWS,
            self.writePixel,
            self.writeFastHLine,
            self.writeFastVLine,
            self.writeFillRect,
            None,
            None,
        )

    @classmethod
    def clearDisplay(self):
        self.ipg.clear()
        self.ipm.clear()

    @classmethod
    def display(self):
        if self.displayMode == 0:
            self.ipm.display()
        elif self.displayMode == 1:
            self.ipg.display()

    @classmethod
    def partialUpdate(self):
        if self.displayMode == 1:
            return
        self.ipp.display()

    # Read the battery voltage. Note that the result depends on the ADC calibration, and be a bit off.
    @classmethod
    def read_battery(cls):
        cls.VBAT_EN.value(0)
        # Probably don't need to delay since Micropython is slow, but we do it anyway
        time.sleep_ms(1)
        value = cls.VBAT.read()
        cls.VBAT_EN.value(1)
        result = (value / 4095.0) * 1.1 * 3.548133892 * 2
        return result

    # Read panel temperature. I varies +- 2 degree
    @classmethod
    def read_temperature(cls):
        # start temperature measurement and wait 5 ms
        cls._i2c.writeto_mem(TPS65186_addr, 0x0D, bytes((0x80,)))
        time.sleep_ms(5)

        # request temperature data from panel
        cls._i2c.writeto(TPS65186_addr, bytearray((0x00,)))
        cls._temperature = cls._i2c.readfrom(TPS65186_addr, 1)

        # convert data from bytes to integer
        cls.temperatureInt = int.from_bytes(cls._temperature, "big", True)
        return cls.temperatureInt

    # _tps65186_write writes an 8-bit value to a register
    @classmethod
    def _tps65186_write(cls, reg, v):
        cls._i2c.writeto_mem(TPS65186_addr, reg, bytes((v,)))

    # _tps65186_read reads an 8-bit value from a register
    @classmethod
    def _tps65186_read(cls, reg):
        cls._i2c.readfrom_mem(TPS65186_addr, reg, 1)[0]

    # power_on turns the voltage regulator on and wakes up the display (GMODE and OE)
    @classmethod
    def power_on(cls):
        if cls._on:
            return
        cls._on = True
        # turn on power regulator
        cls.TPS_WAKEUP(1)
        cls.TPS_PWRUP(1)
        cls.TPS_VCOM(1)
        # enable all rails
        cls._tps65186_write(0x01, 0x3F)  # ???
        time.sleep_ms(40)
        cls._tps65186_write(0x0D, 0x80)  # ???
        time.sleep_ms(2)
        cls._temperature = cls._tps65186_read(1)
        # wake-up display
        cls.EPD_GMODE(1)
        cls.EPD_OE(1)

    # power_off puts the display to sleep and cuts the power
    # TODO: also tri-state gpio pins to avoid current leakage during deep-sleep
    @classmethod
    def power_off(cls):
        if not cls._on:
            return
        cls._on = False
        # put display to sleep
        cls.EPD_GMODE(0)
        cls.EPD_OE(0)
        # turn off power regulator
        cls.TPS_PWRUP(0)
        cls.TPS_WAKEUP(0)
        cls.TPS_VCOM(0)

    # ===== Methods that are independent of pixel bit depth

    # vscan_start begins a vertical scan by toggling CKV and SPV
    # sleep_us calls are commented out 'cause MP is slow enough...
    @classmethod
    def vscan_start(cls):
        def ckv_pulse():
            cls.EPD_CKV(0)
            cls.EPD_CKV(1)

        # start a vertical scan pulse
        cls.EPD_CKV(1)  # time.sleep_us(7)
        cls.EPD_SPV(0)  # time.sleep_us(10)
        ckv_pulse()  # time.sleep_us(8)
        cls.EPD_SPV(1)  # time.sleep_us(10)
        # pulse through 3 scan lines that end up being invisible
        ckv_pulse()  # time.sleep_us(18)
        ckv_pulse()  # time.sleep_us(18)
        ckv_pulse()

    # vscan_write latches the row into the display pixels and moves to the next row
    @micropython.viper
    @staticmethod
    def vscan_write():
        w1ts0 = ptr32(int(ESP32_GPIO + 4 * W1TS0))
        w1tc0 = ptr32(int(ESP32_GPIO + 4 * W1TC0))
        w1tc0[W1TC1 - W1TC0] = EPD_CKV  # remove gate drive
        w1ts0[0] = EPD_LE  # pulse to latch row --
        w1ts0[0] = EPD_LE  # delay a tiny bit
        w1tc0[0] = EPD_LE
        w1tc0[0] = EPD_LE  # delay a tiny bit
        w1ts0[W1TS1 - W1TS0] = EPD_CKV  # apply gate drive to next row

    # byte2gpio converts a byte of data for the screen to 32 bits of gpio0..31
    # (oh, e-radionica, why didn't you group the gpios better?!)
    byte2gpio = []

    @classmethod
    def gen_byte2gpio(cls):
        cls.byte2gpio = array("L", bytes(4 * 256))
        for b in range(256):
            cls.byte2gpio[b] = (
                (b & 0x3) << 4 | (b & 0xC) << 16 | (
                    b & 0x10) << 19 | (b & 0xE0) << 20
            )
        # sanity check that all EPD_DATA bits got set at some point and no more
        union = 0
        for i in range(256):
            union |= cls.byte2gpio[i]
        assert union == EPD_DATA

    # fill_screen writes the same value to all bytes of the screen, it is used for cleaning
    @micropython.viper
    @staticmethod
    def fill_screen(data: int):
        w1ts0 = ptr32(int(ESP32_GPIO + 4 * W1TS0))
        w1tc0 = ptr32(int(ESP32_GPIO + 4 * W1TC0))
        # set the data output gpios
        w1tc0[0] = EPD_DATA | EPD_CL
        w1ts0[0] = data
        vscan_write = _Inkplate.vscan_write
        epd_cl = EPD_CL

        # send all rows
        for r in range(D_ROWS):
            # send first byte of row with start-row signal
            w1tc0[W1TC1 - W1TC0] = EPD_SPH
            w1ts0[0] = epd_cl
            w1tc0[0] = epd_cl
            w1ts0[W1TS1 - W1TS0] = EPD_SPH

            # send remaining bytes (we overshoot by one, which is OK)
            i = int(D_COLS >> 3)
            while i > 0:
                w1ts0[0] = epd_cl
                w1tc0[0] = epd_cl
                w1ts0[0] = epd_cl
                w1tc0[0] = epd_cl
                i -= 1

            # latch row and increment to next
            # inlined vscan_write()
            w1tc0[W1TC1 - W1TC0] = EPD_CKV  # remove gate drive
            w1ts0[0] = EPD_LE  # pulse to latch row --
            w1ts0[0] = EPD_LE  # delay a tiny bit
            w1tc0[0] = EPD_LE
            w1tc0[0] = EPD_LE  # delay a tiny bit
            w1ts0[W1TS1 - W1TS0] = EPD_CKV  # apply gate drive to next row

    # clean fills the screen with one of the four possible pixel patterns
    @classmethod
    def clean(cls, patt, rep):
        c = [0xAA, 0x55, 0x00, 0xFF][patt]
        data = _Inkplate.byte2gpio[c] & ~EPD_CL
        for i in range(rep):
            cls.vscan_start()
            cls.fill_screen(data)


class InkplateMono(framebuf.FrameBuffer):
    def __init__(self):
        self._framebuf = bytearray(D_ROWS * D_COLS // 8)
        super().__init__(self._framebuf, D_COLS, D_ROWS, framebuf.MONO_HMSB)
        ip = InkplateMono
        ip._gen_luts()
        ip._wave = [ip.lut_blk, ip.lut_blk, ip.lut_blk,
                    ip.lut_blk, ip.lut_blk, ip.lut_bw]

    # gen_luts generates the look-up tables to convert a nibble (4 bits) of pixels to the
    # 32-bits that need to be pushed into the gpio port.
    # The LUTs used here were copied from the e-Radionica Inkplate-6-Arduino-library.
    @classmethod
    def _gen_luts(cls):
        # is there a better way to init an array with 16 words???
        b16 = bytes(4 * 16)
        # bits to ship to gpio to make pixels white
        cls.lut_wht = array("L", b16)
        # bits to ship to gpio to make pixels black
        cls.lut_blk = array("L", b16)
        # bits to ship to gpio to make pixels black and white
        cls.lut_bw = array("L", b16)
        for i in range(16):
            wht = 0
            blk = 0
            bw = 0
            # display uses 2 bits per pixel: 00=discharge, 01=black, 10=white, 11=skip
            for bit in range(4):
                wht = wht | ((2 if (i >> bit) & 1 == 0 else 3) << (2 * bit))
                blk = blk | ((1 if (i >> bit) & 1 == 1 else 3) << (2 * bit))
                bw = bw | ((1 if (i >> bit) & 1 == 1 else 2) << (2 * bit))
            cls.lut_wht[i] = _Inkplate.byte2gpio[wht] | EPD_CL
            cls.lut_blk[i] = _Inkplate.byte2gpio[blk] | EPD_CL
            cls.lut_bw[i] = _Inkplate.byte2gpio[bw] | EPD_CL
        # print("Black: %08x, White:%08x Data:%08x" % (cls.lut_bw[0xF], cls.lut_bw[0], EPD_DATA))

    # _send_row writes a row of data to the display
    @micropython.viper
    @staticmethod
    def _send_row(lut_in, framebuf, row: int):
        ROW_LEN = D_COLS >> 3  # length of row in bytes
        # cache vars into locals
        w1ts0 = ptr32(int(ESP32_GPIO + 4 * W1TS0))
        w1tc0 = ptr32(int(ESP32_GPIO + 4 * W1TC0))
        off = int(EPD_DATA | EPD_CL)  # mask with all data bits and clock bit
        fb = ptr8(framebuf)
        ix = int(row * ROW_LEN + ROW_LEN - 1)  # index into framebuffer
        lut = ptr32(lut_in)
        # send first byte
        data = int(fb[ix])
        ix -= 1
        w1tc0[0] = off
        w1tc0[W1TC1 - W1TC0] = EPD_SPH
        w1ts0[0] = lut[data >> 4]  # set data bits and assert clock
        # w1tc0[0] = EPD_CL  # clear clock, leaving data bits (unreliable if data also cleared)
        w1tc0[0] = off  # clear data bits as well ready for next byte
        w1ts0[W1TS1 - W1TS0] = EPD_SPH
        w1ts0[0] = lut[data & 0xF]
        # w1tc0[0] = EPD_CL
        w1tc0[0] = off
        # send the remaining bytes
        for c in range(ROW_LEN - 1):
            data = int(fb[ix])
            ix -= 1
            w1ts0[0] = lut[data >> 4]
            # w1tc0[0] = EPD_CL
            w1tc0[0] = off
            w1ts0[0] = lut[data & 0xF]
            # w1tc0[0] = EPD_CL
            w1tc0[0] = off

    # display_mono sends the monochrome buffer to the display, clearing it first
    def display(self):
        ip = _Inkplate
        ip.power_on()

        # clean the display
        t0 = time.ticks_ms()
        ip.clean(0, 1)
        ip.clean(1, 12)
        ip.clean(2, 1)
        ip.clean(0, 11)
        ip.clean(2, 1)
        ip.clean(1, 12)
        ip.clean(2, 1)
        ip.clean(0, 11)

        # the display gets written N times
        t1 = time.ticks_ms()
        n = 0
        send_row = InkplateMono._send_row
        vscan_write = ip.vscan_write
        fb = self._framebuf
        for lut in self._wave:
            ip.vscan_start()
            # write all rows
            r = D_ROWS - 1
            while r >= 0:
                send_row(lut, fb, r)
                vscan_write()
                r -= 1
            n += 1

        t2 = time.ticks_ms()
        tc = time.ticks_diff(t1, t0)
        td = time.ticks_diff(t2, t1)
        tt = time.ticks_diff(t2, t0)
        print(
            "Mono: clean %dms (%dms ea), draw %dms (%dms ea), total %dms"
            % (tc, tc // (4 + 22 + 24), td, td // len(self._wave), tt)
        )

        ip.clean(2, 2)
        ip.clean(3, 1)
        ip.power_off()

    # @micropython.viper
    def clear(self):
        self.fill(0)
        # fb = ptr8(self._framebuf)
        # for ix in range(D_ROWS * D_COLS // 8):
        #    fb[ix] = 0


Shapes.__mix_me_in(InkplateMono)

# Inkplate display with 2 bits of gray scale (4 levels)


class InkplateGS2(framebuf.FrameBuffer):
    _wave = None

    def __init__(self):
        self._framebuf = bytearray(D_ROWS * D_COLS // 4)
        super().__init__(self._framebuf, D_COLS, D_ROWS, framebuf.GS2_HMSB)
        if not InkplateGS2._wave:
            InkplateGS2._gen_wave()

    # _gen_wave generates the waveform table. The table consists of N phases or steps during
    # each of which the entire display gets written. The array in each phase gets indexed with
    # a nibble of data and contains the 32-bits that need to be pushed into the gpio port.
    # The waveform used here was adapted from the e-Radionica Inkplate-6-Arduino-library
    # by taking colors 0 (black), 3, 5, and 7 (white) from "waveform3Bit[8][7]".
    @classmethod
    def _gen_wave(cls):
        # genlut generates the lookup table that maps a nibble (2 pixels, 4 bits) to a 32-bit
        # word to push into the GPIO port
        def genlut(op):
            return bytes([op[j] | op[i] << 2 for i in range(4) for j in range(4)])

        cls._wave = [genlut(w) for w in WAVE_2B]

    # _send_row writes a row of data to the display
    @micropython.viper
    @staticmethod
    def _send_row(lut_in, framebuf, row: int):
        ROW_LEN = D_COLS >> 2  # length of row in bytes
        # cache vars into locals
        w1ts0 = ptr32(int(ESP32_GPIO + 4 * W1TS0))
        w1tc0 = ptr32(int(ESP32_GPIO + 4 * W1TC0))
        off = int(EPD_DATA | EPD_CL)  # mask with all data bits and clock bit
        fb = ptr8(framebuf)
        ix = int(row * ROW_LEN + (ROW_LEN - 1))  # index into framebuffer
        lut = ptr8(lut_in)
        b2g = ptr32(_Inkplate.byte2gpio)
        # send first byte
        data = int(fb[ix])
        ix -= 1
        w1tc0[0] = off
        w1tc0[W1TC1 - W1TC0] = EPD_SPH
        w1ts0[0] = b2g[lut[data >> 4] << 4 | lut[data & 0xF]
                       ] | EPD_CL  # set data bits and clock
        # w1tc0[0] = EPD_CL  # clear clock, leaving data bits (unreliable if data also cleared)
        w1tc0[0] = off  # clear data bits as well ready for next byte
        w1ts0[W1TS1 - W1TS0] = EPD_SPH
        # send the remaining bytes
        for c in range(ROW_LEN - 1):
            data = int(fb[ix])
            ix -= 1
            w1ts0[0] = b2g[lut[data >> 4] << 4 | lut[data & 0xF]] | EPD_CL
            # w1tc0[0] = EPD_CL
            w1tc0[0] = off

    # display_mono sends the monochrome buffer to the display, clearing it first
    def display(self):
        ip = _Inkplate
        ip.power_on()

        # clean the display
        t0 = time.ticks_ms()
        ip.clean(0, 1)
        ip.clean(1, 12)
        ip.clean(2, 1)
        ip.clean(0, 11)
        ip.clean(2, 1)
        ip.clean(1, 12)
        ip.clean(2, 1)
        ip.clean(0, 11)

        # the display gets written N times
        t1 = time.ticks_ms()
        n = 0
        send_row = InkplateGS2._send_row
        vscan_write = ip.vscan_write
        fb = self._framebuf
        for lut in InkplateGS2._wave:
            ip.vscan_start()
            # write all rows
            r = D_ROWS - 1
            while r >= 0:
                send_row(lut, fb, r)
                vscan_write()
                r -= 1
            n += 1

        t2 = time.ticks_ms()
        tc = time.ticks_diff(t1, t0)
        td = time.ticks_diff(t2, t1)
        tt = time.ticks_diff(t2, t0)
        print(
            "GS2: clean %dms (%dms ea), draw %dms (%dms ea), total %dms"
            % (tc, tc // (4 + 22 + 24), td, td // len(InkplateGS2._wave), tt)
        )

        ip.clean(2, 1)  # ??
        ip.clean(3, 1)
        ip.power_off()

    # @micropython.viper
    def clear(self):
        self.fill(3)
        # fb = ptr8(self._framebuf)
        # for ix in range(int(len(self._framebuf))):
        #    fb[ix] = 0xFF


Shapes.__mix_me_in(InkplateGS2)

# InkplatePartial managed partial updates. It starts by making a copy of the current framebuffer
# and then when asked to draw it renders the differences between the copy and the new framebuffer
# state. The constructor needs a reference to the current/main display object (InkplateMono).
# Only InkplateMono is supported at the moment.


class InkplatePartial:
    def __init__(self, base):
        self._base = base
        self._framebuf = bytearray(len(base._framebuf))
        InkplatePartial._gen_lut_mono()

    # start makes a reference copy of the current framebuffer
    def start(self):
        self._framebuf[:] = self._base._framebuf[:]

    # display the changes between our reference copy and the current framebuffer contents
    def display(self, x=0, y=0, w=D_COLS, h=D_ROWS):
        ip = _Inkplate
        ip.power_on()

        # the display gets written a couple of times
        t0 = time.ticks_ms()
        n = 0
        send_row = InkplatePartial._send_row
        skip_rows = InkplatePartial._skip_rows
        vscan_write = ip.vscan_write
        nfb = self._base._framebuf  # new framebuffer
        ofb = self._framebuf  # old framebuffer
        lut = InkplatePartial._lut_mono
        h -= 1
        for _ in range(5):
            ip.vscan_start()
            r = D_ROWS - 1
            # skip rows that supposedly have no change
            if r > y + h:
                skip_rows(r - (y + h))
                r = y + h
            # write changed rows
            while r >= y:
                send_row(lut, ofb, nfb, r)
                vscan_write()
                r -= 1
            # skip remaining rows (doesn't seem to be necessary for Inkplate 6 but it is for 10)
            if r > 0:
                skip_rows(r)
            n += 1

        t1 = time.ticks_ms()
        td = time.ticks_diff(t1, t0)
        print(
            "Partial: draw %dms (%dms/frame %dus/row) (y=%d..%d)"
            % (td, td // n, td * 1000 // n // (D_ROWS - y), y, y + h + 1)
        )

        ip.clean(2, 2)
        ip.clean(3, 1)
        ip.power_off()

    # gen_lut_mono generates a look-up tables to change the display from a nibble of old
    # pixels (4 bits = 4 pixels) to a nibble of new pixels. The LUT contains the
    # 32-bits that need to be pushed into the gpio port to effect the change.
    @classmethod
    def _gen_lut_mono(cls):
        lut = cls._lut_mono = array("L", bytes(4 * 256))
        for o in range(16):  # iterate through all old-pixels combos
            for n in range(16):  # iterate through all new-pixels combos
                bw = 0
                for bit in range(4):
                    # value to send to display: turns out that if we juxtapose the old and new
                    # bits we get the right value except for the 00 combination...
                    val = (((o >> bit) << 1) & 2) | ((n >> bit) & 1)
                    if val == 0:
                        val = 3
                    bw = bw | (val << (2 * bit))
                lut[o * 16 + n] = _Inkplate.byte2gpio[bw] | EPD_CL
        # print("Black: %08x, White:%08x Data:%08x" % (cls.lut_bw[0xF], cls.lut_bw[0], EPD_DATA))

    # _skip_rows skips N rows
    @micropython.viper
    @staticmethod
    def _skip_rows(rows: int):
        if rows <= 0:
            return
        # cache vars into locals
        w1ts0 = ptr32(int(ESP32_GPIO + 4 * W1TS0))
        w1tc0 = ptr32(int(ESP32_GPIO + 4 * W1TC0))

        # need to fill the column latches with "no-change" values (all ones)
        epd_cl = EPD_CL
        w1tc0[0] = epd_cl
        w1ts0[0] = EPD_DATA
        # send first byte of row with start-row signal
        w1tc0[W1TC1 - W1TC0] = EPD_SPH
        w1ts0[0] = epd_cl
        w1tc0[0] = epd_cl
        w1ts0[W1TS1 - W1TS0] = EPD_SPH
        # send remaining bytes
        i = int(D_COLS >> 3)
        while i > 0:
            w1ts0[0] = epd_cl
            w1tc0[0] = epd_cl
            w1ts0[0] = epd_cl
            w1tc0[0] = epd_cl
            i -= 1

        # write the same row over and over, weird thing is that we need the sleep otherwise
        # the rows we subsequently draw don't draw proper whites leaving ghosts behind - hard to
        # understand why the speed at which we "skip" rows affects rows that are drawn later...
        while rows > 0:
            _Inkplate.vscan_write()
            rows -= 1
            time.sleep_us(50)

    # _send_row writes a row of data to the display
    @micropython.viper
    @staticmethod
    def _send_row(lut_in, old_framebuf, new_framebuf, row: int):
        ROW_LEN = D_COLS >> 3  # length of row in bytes
        # cache vars into locals
        w1ts0 = ptr32(int(ESP32_GPIO + 4 * W1TS0))
        w1tc0 = ptr32(int(ESP32_GPIO + 4 * W1TC0))
        off = int(EPD_DATA | EPD_CL)  # mask with all data bits and clock bit
        ofb = ptr8(old_framebuf)
        nfb = ptr8(new_framebuf)
        ix = int(row * ROW_LEN + (ROW_LEN - 1))  # index into framebuffer
        lut = ptr32(lut_in)
        # send first byte
        odata = int(ofb[ix])
        ndata = int(nfb[ix])
        ix -= 1
        w1tc0[0] = off
        w1tc0[W1TC1 - W1TC0] = EPD_SPH
        if odata == ndata:
            w1ts0[0] = off  # send all-ones: no change to any of the pixels
            w1tc0[0] = EPD_CL
            w1ts0[W1TS1 - W1TS0] = EPD_SPH
            w1ts0[0] = EPD_CL
            w1tc0[0] = off
        else:
            w1ts0[0] = lut[(odata & 0xF0) + (ndata >> 4)]
            w1tc0[0] = off  # clear data bits as well ready for next byte
            w1ts0[W1TS1 - W1TS0] = EPD_SPH
            w1ts0[0] = lut[((odata & 0xF) << 4) + (ndata & 0xF)]
            w1tc0[0] = off
        # send the remaining bytes
        for c in range(ROW_LEN - 1):
            odata = int(ofb[ix])
            ndata = int(nfb[ix])
            ix -= 1
            if odata == ndata:
                w1ts0[0] = off  # send all-ones: no change to any of the pixels
                w1tc0[0] = EPD_CL
                w1ts0[0] = EPD_CL
                w1tc0[0] = off
            else:
                w1ts0[0] = lut[(odata & 0xF0) + ((ndata >> 4) & 0xF)]
                w1tc0[0] = off
                w1ts0[0] = lut[((odata & 0xF) << 4) + (ndata & 0xF)]
                w1tc0[0] = off

# Inkplate wraper to make it more easy for use


class Inkplate:
    INKPLATE_1BIT = 0
    INKPLATE_2BIT = 1

    BLACK = 1
    WHITE = 0

    _width = D_COLS
    _height = D_ROWS

    rotation = 0
    displayMode = 0
    textSize = 1

    def __init__(self, mode):
        self.displayMode = mode
        # try:
        #     os.mount(
        #         SDCard(
        #             slot=3,
        #             miso=Pin(12),
        #             mosi=Pin(13),
        #             sck=Pin(14),
        #             cs=Pin(15)),
        #         "/sd"
        #     )
        # except:
        #     print("Sd card could not be read")

    def begin(self):
        _Inkplate.init(I2C(0, scl=Pin(22), sda=Pin(21)))

        self.ipg = InkplateGS2()
        self.ipm = InkplateMono()
        self.ipp = InkplatePartial(self.ipm)

        self.TOUCH1 = _Inkplate.TOUCH1
        self.TOUCH2 = _Inkplate.TOUCH2
        self.TOUCH3 = _Inkplate.TOUCH3

        self.GFX = GFX(
            D_COLS,
            D_ROWS,
            self.writePixel,
            self.writeFastHLine,
            self.writeFastVLine,
            self.writeFillRect,
            None,
            None,
        )

    def clearDisplay(self):
        self.ipm.clear()
        self.ipg.clear()

    def display(self):
        if self.displayMode == 0:
            self.ipm.display()
        elif self.displayMode == 1:
            self.ipg.display()

        self.ipp.start()  # making framebuffer copy for partial update

    def partialUpdate(self):
        if self.displayMode == self.INKPLATE_2BIT:
            return
        self.ipp.display()
        self.ipp.start()  # making framebuffer copy for partial update

    def clean(self):
        self.einkOn()
        _Inkplate.clean(0, 1)
        _Inkplate.clean(1, 12)
        _Inkplate.clean(2, 1)
        _Inkplate.clean(0, 11)
        _Inkplate.clean(2, 1)
        _Inkplate.clean(1, 12)
        _Inkplate.clean(2, 1)
        _Inkplate.clean(0, 11)
        self.einkOff()

    def einkOn(self):
        _Inkplate.power_on()

    def einkOff(self):
        _Inkplate.power_off()

    def readBattery(self):
        return _Inkplate.read_battery()

    def readTemperature(self):
        return _Inkplate.read_temperature()

    def width(self):
        return self._width

    def height(self):
        return self._height

    # Arduino compatibility functions
    def setRotation(self, x):
        self.rotation = x % 4
        if self.rotation == 0 or self.rotation == 2:
            self._width = D_COLS
            self._height = D_ROWS
        elif self.rotation == 1 or self.rotation == 3:
            self._width = D_ROWS
            self._height = D_COLS

    def getRotation(self):
        return self.rotation

    def drawPixel(self, x, y, c):
        self.startWrite()
        self.writePixel(x, y, c)
        self.endWrite()

    def startWrite(self):
        pass

    def writePixel(self, x, y, c):
        if x > self.width() - 1 or y > self.height() - 1 or x < 0 or y < 0:
            return
        if self.rotation == 1:
            x, y = y, x
            x = self.height() - x - 1
        elif self.rotation == 2:
            x = self.width() - x - 1
            y = self.height() - y - 1
        elif self.rotation == 3:
            x, y = y, x
            y = self.width() - y - 1
        (self.ipm.pixel if self.displayMode == self.INKPLATE_1BIT else self.ipg.pixel)(
            x, y, c
        )

    def writeFillRect(self, x, y, w, h, c):
        for j in range(w):
            for i in range(h):
                self.writePixel(x + j, y + i, c)

    def writeFastVLine(self, x, y, h, c):
        for i in range(h):
            self.writePixel(x, y + i, c)

    def writeFastHLine(self, x, y, w, c):
        for i in range(w):
            self.writePixel(x + i, y, c)

    def writeLine(self, x0, y0, x1, y1, c):
        self.GFX.line(x0, y0, x1, y1, c)

    def endWrite(self):
        pass

    def drawFastVLine(self, x, y, h, c):
        self.startWrite()
        self.writeFastVLine(x, y, h, c)
        self.endWrite()

    def drawFastHLine(self, x, y, w, c):
        self.startWrite()
        self.writeFastHLine(x, y, w, c)
        self.endWrite()

    def fillRect(self, x, y, w, h, c):
        self.startWrite()
        self.writeFillRect(x, y, w, h, c)
        self.endWrite()

    def fillScreen(self, c):
        self.fillRect(0, 0, self.width(), self.height(), c)

    def drawLine(self, x0, y0, x1, y1, c):
        self.startWrite()
        self.writeLine(x0, y0, x1, y1, c)
        self.endWrite()

    def drawRect(self, x, y, w, h, c):
        self.GFX.rect(x, y, w, h, c)

    def drawCircle(self, x, y, r, c):
        self.GFX.circle(x, y, r, c)

    def fillCircle(self, x, y, r, c):
        self.GFX.fill_circle(x, y, r, c)

    def drawTriangle(self, x0, y0, x1, y1, x2, y2, c):
        self.GFX.triangle(x0, y0, x1, y1, x2, y2, c)

    def fillTriangle(self, x0, y0, x1, y1, x2, y2, c):
        self.GFX.fill_triangle(x0, y0, x1, y1, x2, y2, c)

    def drawRoundRect(self, x, y, q, h, r, c):
        self.GFX.round_rect(x, y, q, h, r, c)

    def fillRoundRect(self, x, y, q, h, r, c):
        self.GFX.fill_round_rect(x, y, q, h, r, c)

    def setDisplayMode(self, mode):
        self.displayMode = mode

    def selectDisplayMode(self, mode):
        self.displayMode = mode

    def getDisplayMode(self):
        return self.displayMode

    def setTextSize(self, s):
        self.textSize = s

    def setFont(self, f):
        self.GFX.font = f

    def printText(self, x, y, s):
        self.GFX._very_slow_text(x, y, s, self.textSize, 1)

    def drawBitmap(self, x, y, data, w, h):
        byteWidth = (w + 7) // 8
        byte = 0
        self.startWrite()
        for j in range(h):
            for i in range(w):
                if i & 7:
                    byte <<= 1
                else:
                    byte = data[j * byteWidth + i // 8]
                if byte & 0x80:
                    self.writePixel(x + i, y + j, 1)
        self.endWrite()

    def drawImageFile(self, x, y, path, invert=False):
        with open(path, "rb") as f:
            header14 = f.read(14)
            if header14[0] != 0x42 or header14[1] != 0x4D:
                return 0
            header40 = f.read(40)

            w = int(
                (header40[7] << 24)
                + (header40[6] << 16)
                + (header40[5] << 8)
                + header40[4]
            )
            h = int(
                (header40[11] << 24)
                + (header40[10] << 16)
                + (header40[9] << 8)
                + header40[8]
            )
            dataStart = int((header14[11] << 8) + header14[10])

            depth = int((header40[15] << 8) + header40[14])
            totalColors = int((header40[33] << 8) + header40[32])

            rowSize = 4 * ((depth * w + 31) // 32)

            if totalColors == 0:
                totalColors = 1 << depth

            palette = None

            if depth <= 8:
                palette = [0 for i in range(totalColors)]
                p = f.read(totalColors * 4)
                for i in range(totalColors):
                    palette[i] = (
                        54 * p[i * 4] + 183 * p[i * 4 + 1] + 19 * p[i * 4 + 2]
                    ) >> 14
            # print(palette)
            f.seek(dataStart)
            for j in range(h):
                # print(100 * j // h, "% complete")
                buffer = f.read(rowSize)
                for i in range(w):
                    val = 0
                    if depth == 1:
                        px = int(
                            invert
                            ^ (palette[0] < palette[1])
                            ^ bool(buffer[i >> 3] & (1 << (7 - i & 7)))
                        )
                        val = palette[px]
                    elif depth == 4:
                        px = (buffer[i >> 1] & (0x0F if i & 1 == 1 else 0xF0)) >> (
                            0 if i & 1 else 4
                        )
                        val = palette[px]
                        if invert:
                            val = 3 - val
                    elif depth == 8:
                        px = buffer[i]
                        val = palette[px]
                        if invert:
                            val = 3 - val
                    elif depth == 16:
                        px = (buffer[(i << 1) | 1] << 8) | buffer[(i << 1)]

                        r = (px & 0x7C00) >> 7
                        g = (px & 0x3E0) >> 2
                        b = (px & 0x1F) << 3

                        val = (54 * r + 183 * g + 19 * b) >> 14

                        if invert:
                            val = 3 - val
                    elif depth == 24:
                        r = buffer[i * 3]
                        g = buffer[i * 3 + 1]
                        b = buffer[i * 3 + 2]

                        val = (54 * r + 183 * g + 19 * b) >> 14

                        if invert:
                            val = 3 - val
                    elif depth == 32:
                        r = buffer[i * 4]
                        g = buffer[i * 4 + 1]
                        b = buffer[i * 4 + 2]

                        val = (54 * r + 183 * g + 19 * b) >> 14

                        if invert:
                            val = 3 - val

                    if self.getDisplayMode() == self.INKPLATE_1BIT:
                        val >>= 1

                    self.drawPixel(x + i, y + h - j, val)
