# The MIT License (MIT)
#
# Copyright (c) 2018 Kattni Rembor for Adafruit Industries
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
`adafruit_framebuf`
====================================================

CircuitPython pure-python framebuf module, based on the micropython framebuf module.

* Author(s): Melissa LeBlanc-Williams, Kattni Rembor, Tony DiCola, original file
             created by Damien P. George

Implementation Notes
--------------------

**Hardware:**

* `Adafruit SSD1306 OLED displays <https://www.adafruit.com/?q=ssd1306>`_

**Software and Dependencies:**

* Adafruit CircuitPython firmware for the supported boards:
  https://github.com/adafruit/circuitpython/releases

"""

__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_framebuf.git"

import os
import struct

# Framebuf format constants:
MVLSB = 0  # Single bit displays (like SSD1306 OLED)
RGB565 = 1  # 16-bit color displays
GS4_HMSB = 2  # Unimplemented!
MHMSB = 3  # Single bit displays like the Sharp Memory
RGB888 = 4  # Neopixels and Dotstars


class MHMSBFormat:
    """MHMSBFormat"""

    @staticmethod
    def set_pixel(framebuf, x, y, color):
        """Set a given pixel to a color."""
        index = (y * framebuf.stride + x) // 8
        offset = 7 - x & 0x07
        framebuf.buf[index] = (framebuf.buf[index] & ~(0x01 << offset)) | (
            (color != 0) << offset
        )

    @staticmethod
    def get_pixel(framebuf, x, y):
        """Get the color of a given pixel"""
        index = (y * framebuf.stride + x) // 8
        offset = 7 - x & 0x07
        return (framebuf.buf[index] >> offset) & 0x01

    @staticmethod
    def fill(framebuf, color):
        """completely fill/clear the buffer with a color"""
        if color:
            fill = 0xFF
        else:
            fill = 0x00
        for i in range(len(framebuf.buf)):
            framebuf.buf[i] = fill

    @staticmethod
    def fill_rect(framebuf, x, y, width, height, color):
        """Draw a rectangle at the given location, size and color. The ``fill_rect`` method draws
        both the outline and interior."""
        # pylint: disable=too-many-arguments
        for _x in range(x, x + width):
            offset = 7 - _x & 0x07
            for _y in range(y, y + height):
                index = (_y * framebuf.stride + _x) // 8
                framebuf.buf[index] = (framebuf.buf[index] & ~(0x01 << offset)) | (
                    (color != 0) << offset
                )


class MVLSBFormat:
    """MVLSBFormat"""

    @staticmethod
    def set_pixel(framebuf, x, y, color):
        """Set a given pixel to a color."""
        index = (y >> 3) * framebuf.stride + x
        offset = y & 0x07
        framebuf.buf[index] = (framebuf.buf[index] & ~(0x01 << offset)) | (
            (color != 0) << offset
        )

    @staticmethod
    def get_pixel(framebuf, x, y):
        """Get the color of a given pixel"""
        index = (y >> 3) * framebuf.stride + x
        offset = y & 0x07
        return (framebuf.buf[index] >> offset) & 0x01

    @staticmethod
    def fill(framebuf, color):
        """completely fill/clear the buffer with a color"""
        if color:
            fill = 0xFF
        else:
            fill = 0x00
        for i in range(len(framebuf.buf)):
            framebuf.buf[i] = fill

    @staticmethod
    def fill_rect(framebuf, x, y, width, height, color):
        """Draw a rectangle at the given location, size and color. The ``fill_rect`` method draws
        both the outline and interior."""
        # pylint: disable=too-many-arguments
        while height > 0:
            index = (y >> 3) * framebuf.stride + x
            offset = y & 0x07
            for w_w in range(width):
                framebuf.buf[index + w_w] = (
                    framebuf.buf[index + w_w] & ~(0x01 << offset)
                ) | ((color != 0) << offset)
            y += 1
            height -= 1


class RGB888Format:
    """RGB888Format"""

    @staticmethod
    def set_pixel(framebuf, x, y, color):
        """Set a given pixel to a color."""
        index = (y * framebuf.stride + x) * 3
        if isinstance(color, tuple):
            framebuf.buf[index : index + 3] = bytes(color)
        else:
            framebuf.buf[index : index + 3] = bytes(
                ((color >> 16) & 255, (color >> 8) & 255, color & 255)
            )

    @staticmethod
    def get_pixel(framebuf, x, y):
        """Get the color of a given pixel"""
        index = (y * framebuf.stride + x) * 3
        return (
            (framebuf.buf[index] << 16)
            | (framebuf.buf[index + 1] << 8)
            | framebuf.buf[index + 2]
        )

    @staticmethod
    def fill(framebuf, color):
        """completely fill/clear the buffer with a color"""
        fill = (color >> 16) & 255, (color >> 8) & 255, color & 255
        for i in range(0, len(framebuf.buf), 3):
            framebuf.buf[i : i + 3] = bytes(fill)

    @staticmethod
    def fill_rect(framebuf, x, y, width, height, color):
        """Draw a rectangle at the given location, size and color. The ``fill_rect`` method draws
        both the outline and interior."""
        # pylint: disable=too-many-arguments
        fill = (color >> 16) & 255, (color >> 8) & 255, color & 255
        for _x in range(x, x + width):
            for _y in range(y, y + height):
                index = (_y * framebuf.stride + _x) * 3
                framebuf.buf[index : index + 3] = bytes(fill)


class FrameBuffer:
    """FrameBuffer object.

    :param buf: An object with a buffer protocol which must be large enough to contain every
                pixel defined by the width, height and format of the FrameBuffer.
    :param width: The width of the FrameBuffer in pixel
    :param height: The height of the FrameBuffer in pixel
    :param buf_format: Specifies the type of pixel used in the FrameBuffer; permissible values
                        are listed under Constants below. These set the number of bits used to
                        encode a color value and the layout of these bits in ``buf``. Where a
                        color value c is passed to a method, c is  a small integer with an encoding
                        that is dependent on the format of the FrameBuffer.
    :param stride: The number of pixels between each horizontal line of pixels in the
                   FrameBuffer. This defaults to ``width`` but may need adjustments when
                   implementing a FrameBuffer within another larger FrameBuffer or screen. The
                   ``buf`` size must accommodate an increased step size.

    """

    def __init__(self, buf, width, height, buf_format=MVLSB, stride=None):
        # pylint: disable=too-many-arguments
        self.buf = buf
        self.width = width
        self.height = height
        self.stride = stride
        self._font = None
        if self.stride is None:
            self.stride = width
        if buf_format == MVLSB:
            self.format = MVLSBFormat()
        elif buf_format == MHMSB:
            self.format = MHMSBFormat()
        elif buf_format == RGB888:
            self.format = RGB888Format()
        else:
            raise ValueError("invalid format")
        self._rotation = 0

    @property
    def rotation(self):
        """The rotation setting of the display, can be one of (0, 1, 2, 3)"""
        return self._rotation

    @rotation.setter
    def rotation(self, val):
        if not val in (0, 1, 2, 3):
            raise RuntimeError("Bad rotation setting")
        self._rotation = val

    def fill(self, color):
        """Fill the entire FrameBuffer with the specified color."""
        self.format.fill(self, color)

    def fill_rect(self, x, y, width, height, color):
        """Draw a rectangle at the given location, size and color. The ``fill_rect`` method draws
        both the outline and interior."""
        # pylint: disable=too-many-arguments, too-many-boolean-expressions
        self.rect(x, y, width, height, color, fill=True)

    def pixel(self, x, y, color=None):
        """If ``color`` is not given, get the color value of the specified pixel. If ``color`` is
        given, set the specified pixel to the given color."""
        if self.rotation == 1:
            x, y = y, x
            x = self.width - x - 1
        if self.rotation == 2:
            x = self.width - x - 1
            y = self.height - y - 1
        if self.rotation == 3:
            x, y = y, x
            y = self.height - y - 1

        if x < 0 or x >= self.width or y < 0 or y >= self.height:
            return None
        if color is None:
            return self.format.get_pixel(self, x, y)
        self.format.set_pixel(self, x, y, color)
        return None

    def hline(self, x, y, width, color):
        """Draw a horizontal line up to a given length."""
        self.rect(x, y, width, 1, color, fill=True)

    def vline(self, x, y, height, color):
        """Draw a vertical line up to a given length."""
        self.rect(x, y, 1, height, color, fill=True)

    def circle(self, center_x, center_y, radius, color):
        """Draw a circle at the given midpoint location, radius and color.
        The ```circle``` method draws only a 1 pixel outline."""
        x = radius - 1
        y = 0
        d_x = 1
        d_y = 1
        err = d_x - (radius << 1)
        while x >= y:
            self.pixel(center_x + x, center_y + y, color)
            self.pixel(center_x + y, center_y + x, color)
            self.pixel(center_x - y, center_y + x, color)
            self.pixel(center_x - x, center_y + y, color)
            self.pixel(center_x - x, center_y - y, color)
            self.pixel(center_x - y, center_y - x, color)
            self.pixel(center_x + y, center_y - x, color)
            self.pixel(center_x + x, center_y - y, color)
            if err <= 0:
                y += 1
                err += d_y
                d_y += 2
            if err > 0:
                x -= 1
                d_x += 2
                err += d_x - (radius << 1)

    def rect(self, x, y, width, height, color, *, fill=False):
        """Draw a rectangle at the given location, size and color. The ```rect``` method draws only
        a 1 pixel outline."""
        # pylint: disable=too-many-arguments
        if self.rotation == 1:
            x, y = y, x
            width, height = height, width
            x = self.width - x - width
        if self.rotation == 2:
            x = self.width - x - width
            y = self.height - y - height
        if self.rotation == 3:
            x, y = y, x
            width, height = height, width
            y = self.height - y - height

        # pylint: disable=too-many-boolean-expressions
        if (
            width < 1
            or height < 1
            or (x + width) <= 0
            or (y + height) <= 0
            or y >= self.height
            or x >= self.width
        ):
            return
        x_end = min(self.width - 1, x + width - 1)
        y_end = min(self.height - 1, y + height - 1)
        x = max(x, 0)
        y = max(y, 0)
        if fill:
            self.format.fill_rect(self, x, y, x_end - x + 1, y_end - y + 1, color)
        else:
            self.format.fill_rect(self, x, y, x_end - x + 1, 1, color)
            self.format.fill_rect(self, x, y, 1, y_end - y + 1, color)
            self.format.fill_rect(self, x, y_end, x_end - x + 1, 1, color)
            self.format.fill_rect(self, x_end, y, 1, y_end - y + 1, color)

    def line(self, x_0, y_0, x_1, y_1, color):
        # pylint: disable=too-many-arguments
        """Bresenham's line algorithm"""
        d_x = abs(x_1 - x_0)
        d_y = abs(y_1 - y_0)
        x, y = x_0, y_0
        s_x = -1 if x_0 > x_1 else 1
        s_y = -1 if y_0 > y_1 else 1
        if d_x > d_y:
            err = d_x / 2.0
            while x != x_1:
                self.pixel(x, y, color)
                err -= d_y
                if err < 0:
                    y += s_y
                    err += d_x
                x += s_x
        else:
            err = d_y / 2.0
            while y != y_1:
                self.pixel(x, y, color)
                err -= d_x
                if err < 0:
                    x += s_x
                    err += d_y
                y += s_y
        self.pixel(x, y, color)

    def blit(self):
        """blit is not yet implemented"""
        raise NotImplementedError()

    def scroll(self, delta_x, delta_y):
        """shifts framebuf in x and y direction"""
        if delta_x < 0:
            shift_x = 0
            xend = self.width + delta_x
            dt_x = 1
        else:
            shift_x = self.width - 1
            xend = delta_x - 1
            dt_x = -1
        if delta_y < 0:
            y = 0
            yend = self.height + delta_y
            dt_y = 1
        else:
            y = self.height - 1
            yend = delta_y - 1
            dt_y = -1
        while y != yend:
            x = shift_x
            while x != xend:
                self.format.set_pixel(
                    self, x, y, self.format.get_pixel(self, x - delta_x, y - delta_y)
                )
                x += dt_x
            y += dt_y

    # pylint: disable=too-many-arguments
    def text(self, string, x, y, color, *, font_name="font5x8.bin", size=1):
        """Place text on the screen in variables sizes. Breaks on \n to next line.

        Does not break on line going off screen.
        """
        # determine our effective width/height, taking rotation into account
        frame_width = self.width
        frame_height = self.height
        if self.rotation == 1 or self.rotation == 3:
            frame_width, frame_height = frame_height, frame_width

        for chunk in string.split("\n"):
            if not self._font or self._font.font_name != font_name:
                # load the font!
                self._font = BitmapFont(font_name)
            width = self._font.font_width
            height = self._font.font_height
            for i, char in enumerate(chunk):
                char_x = x + (i * (width + 1)) * size
                if (
                    char_x + (width * size) > 0
                    and char_x < frame_width
                    and y + (height * size) > 0
                    and y < frame_height
                ):
                    self._font.draw_char(char, char_x, y, self, color, size=size)
            y += height * size

    # pylint: enable=too-many-arguments

    def image(self, img):
        """Set buffer to value of Python Imaging Library image.  The image should
        be in 1 bit mode and a size equal to the display size."""
        # determine our effective width/height, taking rotation into account
        width = self.width
        height = self.height
        if self.rotation == 1 or self.rotation == 3:
            width, height = height, width

        if isinstance(self.format, RGB888Format) and img.mode != "RGB":
            raise ValueError("Image must be in mode RGB.")
        if isinstance(self.format, (MHMSBFormat, MVLSBFormat)) and img.mode != "1":
            raise ValueError("Image must be in mode 1.")

        imwidth, imheight = img.size
        if imwidth != width or imheight != height:
            raise ValueError(
                "Image must be same dimensions as display ({0}x{1}).".format(
                    width, height
                )
            )
        # Grab all the pixels from the image, faster than getpixel.
        pixels = img.load()
        # Clear buffer
        for i in range(len(self.buf)):
            self.buf[i] = 0
        # Iterate through the pixels
        for x in range(width):  # yes this double loop is slow,
            for y in range(height):  #  but these displays are small!
                if img.mode == "RGB":
                    self.pixel(x, y, pixels[(x, y)])
                elif pixels[(x, y)]:
                    self.pixel(x, y, 1)  # only write if pixel is true


# MicroPython basic bitmap font renderer.
# Author: Tony DiCola
# License: MIT License (https://opensource.org/licenses/MIT)
class BitmapFont:
    """A helper class to read binary font tiles and 'seek' through them as a
    file to display in a framebuffer. We use file access so we dont waste 1KB
    of RAM on a font!"""

    def __init__(self, font_name="font5x8.bin"):
        # Specify the drawing area width and height, and the pixel function to
        # call when drawing pixels (should take an x and y param at least).
        # Optionally specify font_name to override the font file to use (default
        # is font5x8.bin).  The font format is a binary file with the following
        # format:
        # - 1 unsigned byte: font character width in pixels
        # - 1 unsigned byte: font character height in pixels
        # - x bytes: font data, in ASCII order covering all 255 characters.
        #            Each character should have a byte for each pixel column of
        #            data (i.e. a 5x8 font has 5 bytes per character).
        self.font_name = font_name

        # Open the font file and grab the character width and height values.
        # Note that only fonts up to 8 pixels tall are currently supported.
        try:
            self._font_code=b'\x05\x08\x00\x00\x00\x00\x00>[O[>>kOk>\x1c>|>\x1c\x18<~<\x18\x1cW}W\x1c\x1c^\x7f^\x1c\x00\x18<\x18\x00\xff\xe7\xc3\xe7\xff\x00\x18$\x18\x00\xff\xe7\xdb\xe7\xff0H:\x06\x0e&)y)&@\x7f\x05\x05\x07@\x7f\x05%?Z<\xe7<Z\x7f>\x1c\x1c\x08\x08\x1c\x1c>\x7f\x14"\x7f"\x14__\x00__\x06\t\x7f\x01\x7f\x00f\x89\x95j`````\x94\xa2\xff\xa2\x94\x08\x04~\x04\x08\x10 ~ \x10\x08\x08*\x1c\x08\x08\x1c*\x08\x08\x1e\x10\x10\x10\x10\x0c\x1e\x0c\x1e\x0c08>80\x06\x0e>\x0e\x06\x00\x00\x00\x00\x00\x00\x00_\x00\x00\x00\x07\x00\x07\x00\x14\x7f\x14\x7f\x14$*\x7f*\x12#\x13\x08db6IV P\x00\x08\x07\x03\x00\x00\x1c"A\x00\x00A"\x1c\x00*\x1c\x7f\x1c*\x08\x08>\x08\x08\x00\x80p0\x00\x08\x08\x08\x08\x08\x00\x00``\x00 \x10\x08\x04\x02>QIE>\x00B\x7f@\x00rIIIF!AIM3\x18\x14\x12\x7f\x10\'EEE9<JII1A!\x11\t\x076III6FII)\x1e\x00\x00\x14\x00\x00\x00@4\x00\x00\x00\x08\x14"A\x14\x14\x14\x14\x14\x00A"\x14\x08\x02\x01Y\t\x06>A]YN|\x12\x11\x12|\x7fIII6>AAA"\x7fAAA>\x7fIIIA\x7f\t\t\t\x01>AAQs\x7f\x08\x08\x08\x7f\x00A\x7fA\x00 @A?\x01\x7f\x08\x14"A\x7f@@@@\x7f\x02\x1c\x02\x7f\x7f\x04\x08\x10\x7f>AAA>\x7f\t\t\t\x06>AQ!^\x7f\t\x19)F&III2\x03\x01\x7f\x01\x03?@@@?\x1f @ \x1f?@8@?c\x14\x08\x14c\x03\x04x\x04\x03aYIMC\x00\x7fAAA\x02\x04\x08\x10 \x00AAA\x7f\x04\x02\x01\x02\x04@@@@@\x00\x03\x07\x08\x00 TTx@\x7f(DD88DDD(8DD(\x7f8TTT\x18\x00\x08~\t\x02\x18\xa4\xa4\x9cx\x7f\x08\x04\x04x\x00D}@\x00 @@=\x00\x7f\x10(D\x00\x00A\x7f@\x00|\x04x\x04x|\x08\x04\x04x8DDD8\xfc\x18$$\x18\x18$$\x18\xfc|\x08\x04\x04\x08HTTT$\x04\x04?D$<@@ |\x1c @ \x1c<@0@<D(\x10(DL\x90\x90\x90|DdTLD\x00\x086A\x00\x00\x00w\x00\x00\x00A6\x08\x00\x02\x01\x02\x04\x02<&#&<\x1e\xa1\xa1a\x12:@@ z8TTUY!UUyA"TTxB!UTx@ TUy@\x0c\x1eRr\x129UUUY9TTTY9UTTX\x00\x00E|A\x00\x02E}B\x00\x01E|@}\x12\x11\x12}\xf0(%(\xf0|TUE\x00 TT|T|\n\t\x7fI2III2:DDD:2JHH0:AA!z:B@ x\x00\x9d\xa0\xa0}=BBB==@@@=<$\xff$$H~ICf+/\xfc/+\xff\t)\xf6 \xc0\x88~\t\x03 TTyA\x00\x00D}A0HHJ28@@"z\x00z\n\nr}\r\x191}&))/(&)))&0HM@ 8\x08\x08\x08\x08\x08\x08\x08\x088/\x10\xc8\xac\xba/\x10(4\xfa\x00\x00{\x00\x00\x08\x14*\x14""\x14*\x14\x08U\x00U\x00U\xaaU\xaaU\xaa\xffU\xffU\xff\x00\x00\x00\xff\x00\x10\x10\x10\xff\x00\x14\x14\x14\xff\x00\x10\x10\xff\x00\xff\x10\x10\xf0\x10\xf0\x14\x14\x14\xfc\x00\x14\x14\xf7\x00\xff\x00\x00\xff\x00\xff\x14\x14\xf4\x04\xfc\x14\x14\x17\x10\x1f\x10\x10\x1f\x10\x1f\x14\x14\x14\x1f\x00\x10\x10\x10\xf0\x00\x00\x00\x00\x1f\x10\x10\x10\x10\x1f\x10\x10\x10\x10\xf0\x10\x00\x00\x00\xff\x10\x10\x10\x10\x10\x10\x10\x10\x10\xff\x10\x00\x00\x00\xff\x14\x00\x00\xff\x00\xff\x00\x00\x1f\x10\x17\x00\x00\xfc\x04\xf4\x14\x14\x17\x10\x17\x14\x14\xf4\x04\xf4\x00\x00\xff\x00\xf7\x14\x14\x14\x14\x14\x14\x14\xf7\x00\xf7\x14\x14\x14\x17\x14\x10\x10\x1f\x10\x1f\x14\x14\x14\xf4\x14\x10\x10\xf0\x10\xf0\x00\x00\x1f\x10\x1f\x00\x00\x00\x1f\x14\x00\x00\x00\xfc\x14\x00\x00\xf0\x10\xf0\x10\x10\xff\x10\xff\x14\x14\x14\xff\x14\x10\x10\x10\x1f\x00\x00\x00\x00\xf0\x10\xff\xff\xff\xff\xff\xf0\xf0\xf0\xf0\xf0\xff\xff\xff\x00\x00\x00\x00\x00\xff\xff\x0f\x0f\x0f\x0f\x0f8DD8D\xfcJJJ4~\x02\x02\x06\x06\x02~\x02~\x02cUIAc8DD<\x04@~ \x1e \x06\x02~\x02\x02\x99\xa5\xe7\xa5\x99\x1c*I*\x1cLr\x01rL0JMM00HxH0\xbcbZF=>III\x00~\x01\x01\x01~*****DD_DD@QJD@@DJQ@\x00\x00\xff\x01\x03\xe0\x80\xff\x00\x00\x08\x08kk\x086\x126$6\x06\x0f\t\x0f\x06\x00\x00\x18\x18\x00\x00\x00\x10\x10\x000@\xff\x01\x01\x00\x1f\x01\x01\x1e\x00\x19\x1d\x17\x12\x00<<<<\x00\x00\x00\x00\x00'
            self.font_width, self.font_height = struct.unpack("BB", b'\x05\x08')
            # simple font file validation check based on expected file size
            #if 2 + 256 * self.font_width != os.stat(font_name)[6]:
            #    raise RuntimeError("Invalid font file: " + font_name)
        except OSError:
            print("Could not find font file", font_name)
            raise
        except OverflowError:
            # os.stat can throw this on boards without long int support
            # just hope the font file is valid and press on
            pass

    def deinit(self):
        """Close the font file as cleanup."""
        #self._font.close()

    def __enter__(self):
        """Initialize/open the font file"""
        self.__init__()
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        """cleanup on exit"""
        #self.deinit()
        pass

    def draw_char(
        self, char, x, y, framebuffer, color, size=1
    ):  # pylint: disable=too-many-arguments
        """Draw one character at position (x,y) to a framebuffer in a given color"""
        size = max(size, 1)
        # Don't draw the character if it will be clipped off the visible area.
        # if x < -self.font_width or x >= framebuffer.width or \
        #   y < -self.font_height or y >= framebuffer.height:
        #    return
        # Go through each column of the character.
        for char_x in range(self.font_width):
            # Grab the byte for the current column of font data.
            #self._font.seek(2 + (ord(char) * self.font_width) + char_x)
            chcode=bytes([self._font_code[(2 + (ord(char) * self.font_width) + char_x)]]) 
            try:
                line = struct.unpack("B", chcode)[0]
            except RuntimeError:
                continue  # maybe character isnt there? go to next
            # Go through each row in the column byte.
            for char_y in range(self.font_height):
                # Draw a pixel for each bit that's flipped on.
                if (line >> char_y) & 0x1:
                    framebuffer.fill_rect(
                        x + char_x * size, y + char_y * size, size, size, color
                    )

    def width(self, text):
        """Return the pixel width of the specified text message."""
        return len(text) * (self.font_width + 1)


class FrameBuffer1(FrameBuffer):  # pylint: disable=abstract-method
    """FrameBuffer1 object. Inherits from FrameBuffer."""
