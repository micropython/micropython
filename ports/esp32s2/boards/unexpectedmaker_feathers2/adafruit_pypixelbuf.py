# The MIT License (MIT)
#
# Based on the Adafruit NeoPixel and Adafruit Dotstar CircuitPython drivers.
# Copyright (c) 2019-2020 Roy Hooper
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
`adafruit_pypixelbuf` - A pure python implementation of _pixelbuf
=================================================================
This class is used when _pixelbuf is not available in CircuitPython.  It is based on the work
in neopixel.py and adafruit_dotstar.py.

* Author(s): Damien P. George &  Limor Fried & Scott Shawcroft & Roy Hooper
"""

DOTSTAR_LED_START_FULL_BRIGHT = 0xFF
DOTSTAR_LED_START = 0b11100000  # Three "1" bits, followed by 5 brightness bits
DOTSTAR_LED_BRIGHTNESS = 0b00011111


class PixelBuf:  # pylint: disable=too-many-instance-attributes
    """
    A sequence of RGB/RGBW pixels.

    This is the pure python implementation of CircuitPython's _pixelbuf.

    :param ~int n: Number of pixels
    :param ~str byteorder: Byte order string constant (also sets bpp)
    :param ~float brightness: Brightness (0 to 1.0, default 1.0)
    :param ~bool auto_write: Whether to automatically write pixels (Default False)
    :param bytes header: Sequence of bytes to always send before pixel values.
    :param bytes trailer: Sequence of bytes to always send after pixel values.
    """

    def __init__(  # pylint: disable=too-many-locals,too-many-arguments
        self,
        n,
        byteorder="BGR",
        brightness=1.0,
        auto_write=False,
        header=None,
        trailer=None,
    ):

        bpp, byteorder_tuple, has_white, dotstar_mode = self.parse_byteorder(byteorder)

        self.auto_write = False

        effective_bpp = 4 if dotstar_mode else bpp
        _bytes = effective_bpp * n
        buf = bytearray(_bytes)
        offset = 0

        if header is not None:
            if not isinstance(header, bytearray):
                raise TypeError("header must be a bytearray")
            buf = header + buf
            offset = len(header)

        if trailer is not None:
            if not isinstance(trailer, bytearray):
                raise TypeError("trailer must be a bytearray")
            buf += trailer

        self._pixels = n
        self._bytes = _bytes
        self._byteorder = byteorder_tuple
        self._byteorder_string = byteorder
        self._has_white = has_white
        self._bpp = bpp
        self._pre_brightness_buffer = None
        self._post_brightness_buffer = buf
        self._offset = offset
        self._dotstar_mode = dotstar_mode
        self._pixel_step = effective_bpp

        if dotstar_mode:
            self._byteorder_tuple = (
                byteorder_tuple[0] + 1,
                byteorder_tuple[1] + 1,
                byteorder_tuple[2] + 1,
                0,
            )
            # Initialize the buffer with the dotstar start bytes.
            for i in range(self._offset, self._bytes + self._offset, 4):
                self._post_brightness_buffer[i] = DOTSTAR_LED_START_FULL_BRIGHT

        self._brightness = 1.0
        self.brightness = brightness

        self.auto_write = auto_write

    @staticmethod
    def parse_byteorder(byteorder):
        """
        Parse a Byteorder string for validity and determine bpp, byte order, and
        dostar brightness bits.

        Byteorder strings may contain the following characters:
            R - Red
            G - Green
            B - Blue
            W - White
            P - PWM (PWM Duty cycle for pixel - dotstars 0 - 1.0)

        :param: ~str bpp: bpp string.
        :return: ~tuple: bpp, byteorder, has_white, dotstar_mode
        """
        bpp = len(byteorder)
        dotstar_mode = False
        has_white = False

        if byteorder.strip("RGBWP") != "":
            raise ValueError("Invalid Byteorder string")

        try:
            r = byteorder.index("R")
            g = byteorder.index("G")
            b = byteorder.index("B")
        except ValueError:
            raise ValueError("Invalid Byteorder string")
        if "W" in byteorder:
            w = byteorder.index("W")
            byteorder = (r, g, b, w)
            has_white = True
        elif "P" in byteorder:
            lum = byteorder.index("P")
            byteorder = (r, g, b, lum)
            dotstar_mode = True
        else:
            byteorder = (r, g, b)

        return bpp, byteorder, has_white, dotstar_mode

    @property
    def bpp(self):
        """
        The number of bytes per pixel in the buffer (read-only).
        """
        return self._bpp

    @property
    def brightness(self):
        """
        Float value between 0 and 1.  Output brightness.

        When brightness is less than 1.0, a second buffer will be used to store the color values
        before they are adjusted for brightness.
        """
        return self._brightness

    @brightness.setter
    def brightness(self, value):
        value = min(max(value, 0.0), 1.0)
        change = value - self._brightness
        if -0.001 < change < 0.001:
            return

        self._brightness = value

        if self._pre_brightness_buffer is None:
            self._pre_brightness_buffer = bytearray(self._post_brightness_buffer)

        # Adjust brightness of existing pixels
        offset_check = self._offset % self._pixel_step
        for i in range(self._offset, self._bytes + self._offset):
            # Don't adjust per-pixel luminance bytes in dotstar mode
            if self._dotstar_mode and (i % 4 != offset_check):
                continue
            self._post_brightness_buffer[i] = int(
                self._pre_brightness_buffer[i] * self._brightness
            )

        if self.auto_write:
            self.show()

    @property
    def byteorder(self):
        """
        ByteOrder string for the buffer (read-only)
        """
        return self._byteorder_string

    def __len__(self):
        """
        Number of pixels.
        """
        return self._pixels

    def show(self):
        """
        Call the associated write function to display the pixels
        """
        return self._transmit(self._post_brightness_buffer)

    def fill(self, color):
        """
        Fills the given pixelbuf with the given color.
        :param pixelbuf: A pixel object.
        :param color: Color to set.
        """
        r, g, b, w = self._parse_color(color)
        for i in range(self._pixels):
            self._set_item(i, r, g, b, w)
        if self.auto_write:
            self.show()

    def _parse_color(self, value):
        r = 0
        g = 0
        b = 0
        w = 0
        if isinstance(value, int):
            r = value >> 16
            g = (value >> 8) & 0xFF
            b = value & 0xFF
            w = 0

            if self._dotstar_mode:
                w = 1.0
        else:
            if len(value) < 3 or len(value) > 4:
                raise ValueError(
                    "Expected tuple of length {}, got {}".format(self._bpp, len(value))
                )
            if len(value) == self._bpp:
                if self._bpp == 3:
                    r, g, b = value
                else:
                    r, g, b, w = value
            elif len(value) == 3:
                r, g, b = value
                if self._dotstar_mode:
                    w = 1.0

        if self._bpp == 4:
            if self._dotstar_mode:
                # LED startframe is three "1" bits, followed by 5 brightness bits
                # then 8 bits for each of R, G, and B. The order of those 3 are configurable and
                # vary based on hardware
                w = (int(w * 31) & 0b00011111) | DOTSTAR_LED_START
            elif (
                self._has_white
                and (isinstance(value, int) or len(value) == 3)
                and r == g
                and g == b
            ):
                # If all components are the same and we have a white pixel then use it
                # instead of the individual components when all 4 values aren't explicitly given.
                w = r
                r = 0
                g = 0
                b = 0

        return (r, g, b, w)

    def _set_item(
        self, index, r, g, b, w
    ):  # pylint: disable=too-many-locals,too-many-branches,too-many-arguments
        if index < 0:
            index += len(self)
        if index >= self._pixels or index < 0:
            raise IndexError
        offset = self._offset + (index * self._bpp)

        if self._pre_brightness_buffer is not None:
            if self._bpp == 4:
                self._pre_brightness_buffer[offset + self._byteorder[3]] = w
            self._pre_brightness_buffer[offset + self._byteorder[0]] = r
            self._pre_brightness_buffer[offset + self._byteorder[1]] = g
            self._pre_brightness_buffer[offset + self._byteorder[2]] = b

        if self._bpp == 4:
            # Only apply brightness if w is actually white (aka not DotStar.)
            if not self._dotstar_mode:
                w = int(w * self._brightness)
            self._post_brightness_buffer[offset + self._byteorder[3]] = w

        self._post_brightness_buffer[offset + self._byteorder[0]] = int(
            r * self._brightness
        )
        self._post_brightness_buffer[offset + self._byteorder[1]] = int(
            g * self._brightness
        )
        self._post_brightness_buffer[offset + self._byteorder[2]] = int(
            b * self._brightness
        )

    def __setitem__(self, index, val):
        if isinstance(index, slice):
            start, stop, step = index.indices(self._pixels)
            for val_i, in_i in enumerate(range(start, stop, step)):
                r, g, b, w = self._parse_color(val[val_i])
                self._set_item(in_i, r, g, b, w)
        else:
            r, g, b, w = self._parse_color(val)
            self._set_item(index, r, g, b, w)

        if self.auto_write:
            self.show()

    def _getitem(self, index):
        start = self._offset + (index * self._bpp)
        buffer = (
            self._pre_brightness_buffer
            if self._pre_brightness_buffer is not None
            else self._post_brightness_buffer
        )
        value = [
            buffer[start + self._byteorder[0]],
            buffer[start + self._byteorder[1]],
            buffer[start + self._byteorder[2]],
        ]
        if self._has_white:
            value.append(buffer[start + self._byteorder[3]])
        elif self._dotstar_mode:
            value.append(
                (buffer[start + self._byteorder[3]] & DOTSTAR_LED_BRIGHTNESS) / 31.0
            )
        return value

    def __getitem__(self, index):
        if isinstance(index, slice):
            out = []
            for in_i in range(
                *index.indices(len(self._post_brightness_buffer) // self._bpp)
            ):
                out.append(self._getitem(in_i))
            return out
        if index < 0:
            index += len(self)
        if index >= self._pixels or index < 0:
            raise IndexError
        return self._getitem(index)

    def _transmit(self, buffer):
        raise NotImplementedError("Must be subclassed")


def wheel(pos):
    """
    Helper to create a colorwheel.

    :param pos: int 0-255 of color value to return
    :return: tuple of RGB values
    """
    # Input a value 0 to 255 to get a color value.
    # The colours are a transition r - g - b - back to r.
    if pos < 0 or pos > 255:
        return 0, 0, 0
    if pos < 85:
        return 255 - pos * 3, pos * 3, 0
    if pos < 170:
        pos -= 85
        return 0, 255 - pos * 3, pos * 3
    pos -= 170
    return pos * 3, 0, 255 - pos * 3
