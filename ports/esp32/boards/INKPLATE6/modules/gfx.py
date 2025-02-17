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
`gfx`
====================================================
CircuitPython pixel graphics drawing library.
* Author(s): Kattni Rembor, Tony DiCola, Jonah Yolles-Murphy, based on code by Phil Burgess
Implementation Notes
--------------------
**Hardware:**
**Software and Dependencies:**
* Adafruit CircuitPython firmware for the supported boards:
  https://github.com/adafruit/circuitpython/releases
"""

__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_GFX.git"

# pylint: disable=invalid-name


class GFX:
    # pylint: disable=too-many-instance-attributes
    """Create an instance of the GFX drawing class.
    :param width: The width of the drawing area in pixels.
    :param height: The height of the drawing area in pixels.
    :param pixel: A function to call when a pixel is drawn on the display. This function
                  should take at least an x and y position and then any number of optional
                  color or other parameters.
    :param hline: A function to quickly draw a horizontal line on the display.
                  This should take at least an x, y, and width parameter and
                  any number of optional color or other parameters.
    :param vline: A function to quickly draw a vertical line on the display.
                  This should take at least an x, y, and height paraemter and
                  any number of optional color or other parameters.
    :param fill_rect: A funtion to quickly draw a solid rectangle with four
                  input parameters: x,y, width, and height. Any number of other
                  parameters for color or screen specific data.
    :param text: A function to quickly place text on the screen. The inputs include:
                  x, y data(top left as starting point).
    :param font:  An optional input to augment the default text method with a new font.
                  The input shoudl be a properly formatted dict.
    """
    # pylint: disable=too-many-arguments

    def __init__(
        self,
        width,
        height,
        pixel,
        hline=None,
        vline=None,
        fill_rect=None,
        text=None,
        font=None,
    ):
        # pylint: disable=too-many-instance-attributes
        self.width = width
        self.height = height
        self._pixel = pixel
        # Default to slow horizontal & vertical line implementations if no
        # faster versions are provided.
        if hline is None:
            self.hline = self._slow_hline
        else:
            self.hline = hline
        if vline is None:
            self.vline = self._slow_vline
        else:
            self.vline = vline
        if fill_rect is None:
            self.fill_rect = self._fill_rect
        else:
            self.fill_rect = fill_rect
        if text is None:
            self.text = self._very_slow_text
            # if no supplied font set to std
            if font is None:
                from gfx_standard_font_01 import (  # pylint: disable=import-outside-toplevel # changed
                    text_dict as std_font,
                )

                self.font = std_font
                self.set_text_background()
            else:
                self.font = font
                if not isinstance(self.font, dict):
                    raise ValueError(
                        "Font definitions must be contained in a dictionary object."
                    )
                del self.set_text_background

        else:
            self.text = text

    def pixel(self, x0, y0, *args, **kwargs):
        """A function to pass through in input pixel functionality."""
        # This was added to mainitatn the abstrtion between gfx and the dislay library
        self._pixel(x0, y0, *args, **kwargs)

    def _slow_hline(self, x0, y0, width, *args, **kwargs):
        """Slow implementation of a horizontal line using pixel drawing.
        This is used as the default horizontal line if no faster override
        is provided."""
        if y0 < 0 or y0 > self.height or x0 < -width or x0 > self.width:
            return
        for i in range(width):
            self._pixel(x0 + i, y0, *args, **kwargs)

    def _slow_vline(self, x0, y0, height, *args, **kwargs):
        """Slow implementation of a vertical line using pixel drawing.
        This is used as the default vertical line if no faster override
        is provided."""
        if y0 < -height or y0 > self.height or x0 < 0 or x0 > self.width:
            return
        for i in range(height):
            self._pixel(x0, y0 + i, *args, **kwargs)

    def rect(self, x0, y0, width, height, *args, **kwargs):
        """Rectangle drawing function.  Will draw a single pixel wide rectangle
        starting in the upper left x0, y0 position and width, height pixels in
        size."""
        if y0 < -height or y0 > self.height or x0 < -width or x0 > self.width:
            return
        self.hline(x0, y0, width, *args, **kwargs)
        self.hline(x0, y0 + height - 1, width, *args, **kwargs)
        self.vline(x0, y0, height, *args, **kwargs)
        self.vline(x0 + width - 1, y0, height, *args, **kwargs)

    def _fill_rect(self, x0, y0, width, height, *args, **kwargs):
        """Filled rectangle drawing function.  Will draw a single pixel wide
        rectangle starting in the upper left x0, y0 position and width, height
        pixels in size."""
        if y0 < -height or y0 > self.height or x0 < -width or x0 > self.width:
            return
        for i in range(x0, x0 + width):
            self.vline(i, y0, height, *args, **kwargs)

    def line(self, x0, y0, x1, y1, *args, **kwargs):
        """Line drawing function.  Will draw a single pixel wide line starting at
        x0, y0 and ending at x1, y1."""
        steep = abs(y1 - y0) > abs(x1 - x0)
        if steep:
            x0, y0 = y0, x0
            x1, y1 = y1, x1
        if x0 > x1:
            x0, x1 = x1, x0
            y0, y1 = y1, y0
        dx = x1 - x0
        dy = abs(y1 - y0)
        err = dx // 2
        ystep = 0
        if y0 < y1:
            ystep = 1
        else:
            ystep = -1
        while x0 <= x1:
            if steep:
                self._pixel(y0, x0, *args, **kwargs)
            else:
                self._pixel(x0, y0, *args, **kwargs)
            err -= dy
            if err < 0:
                y0 += ystep
                err += dx
            x0 += 1

    def circle(self, x0, y0, radius, *args, **kwargs):
        """Circle drawing function.  Will draw a single pixel wide circle with
        center at x0, y0 and the specified radius."""
        f = 1 - radius
        ddF_x = 1
        ddF_y = -2 * radius
        x = 0
        y = radius
        self._pixel(x0, y0 + radius, *args, **kwargs)  # bottom
        self._pixel(x0, y0 - radius, *args, **kwargs)  # top
        self._pixel(x0 + radius, y0, *args, **kwargs)  # right
        self._pixel(x0 - radius, y0, *args, **kwargs)  # left
        while x < y:
            if f >= 0:
                y -= 1
                ddF_y += 2
                f += ddF_y
            x += 1
            ddF_x += 2
            f += ddF_x
            # angle notations are based on the unit circle and in diection of being drawn
            self._pixel(x0 + x, y0 + y, *args, **kwargs)  # 270 to 315
            self._pixel(x0 - x, y0 + y, *args, **kwargs)  # 270 to 255
            self._pixel(x0 + x, y0 - y, *args, **kwargs)  # 90 to 45
            self._pixel(x0 - x, y0 - y, *args, **kwargs)  # 90 to 135
            self._pixel(x0 + y, y0 + x, *args, **kwargs)  # 0 to 315
            self._pixel(x0 - y, y0 + x, *args, **kwargs)  # 180 to 225
            self._pixel(x0 + y, y0 - x, *args, **kwargs)  # 0 to 45
            self._pixel(x0 - y, y0 - x, *args, **kwargs)  # 180 to 135

    def fill_circle(self, x0, y0, radius, *args, **kwargs):
        """Filled circle drawing function.  Will draw a filled circule with
        center at x0, y0 and the specified radius."""
        self.vline(x0, y0 - radius, 2 * radius + 1, *args, **kwargs)
        f = 1 - radius
        ddF_x = 1
        ddF_y = -2 * radius
        x = 0
        y = radius
        while x < y:
            if f >= 0:
                y -= 1
                ddF_y += 2
                f += ddF_y
            x += 1
            ddF_x += 2
            f += ddF_x
            self.vline(x0 + x, y0 - y, 2 * y + 1, *args, **kwargs)
            self.vline(x0 + y, y0 - x, 2 * x + 1, *args, **kwargs)
            self.vline(x0 - x, y0 - y, 2 * y + 1, *args, **kwargs)
            self.vline(x0 - y, y0 - x, 2 * x + 1, *args, **kwargs)

    def triangle(self, x0, y0, x1, y1, x2, y2, *args, **kwargs):
        # pylint: disable=too-many-arguments
        """Triangle drawing function.  Will draw a single pixel wide triangle
        around the points (x0, y0), (x1, y1), and (x2, y2)."""
        self.line(x0, y0, x1, y1, *args, **kwargs)
        self.line(x1, y1, x2, y2, *args, **kwargs)
        self.line(x2, y2, x0, y0, *args, **kwargs)

    def fill_triangle(self, x0, y0, x1, y1, x2, y2, *args, **kwargs):
        # Filled triangle drawing function.  Will draw a filled triangle around
        # the points (x0, y0), (x1, y1), and (x2, y2).
        if y0 > y1:
            y0, y1 = y1, y0
            x0, x1 = x1, x0
        if y1 > y2:
            y2, y1 = y1, y2
            x2, x1 = x1, x2
        if y0 > y1:
            y0, y1 = y1, y0
            x0, x1 = x1, x0
        a = 0
        b = 0
        y = 0
        last = 0

        if y0 == y2:
            a = x0
            b = x0
            if x1 < a:
                a = x1
            elif x1 > b:
                b = x1
            if x2 < a:
                a = x2
            elif x2 > b:
                b = x2
            self.hline(a, y0, b-a+1, *args, **kwargs)
            return
        dx01 = x1 - x0
        dy01 = y1 - y0
        dx02 = x2 - x0
        dy02 = y2 - y0
        dx12 = x2 - x1
        dy12 = y2 - y1
        if dy01 == 0:
            dy01 = 1
        if dy02 == 0:
            dy02 = 1
        if dy12 == 0:
            dy12 = 1
        sa = 0
        sb = 0
        if y1 == y2:
            last = y1
        else:
            last = y1-1

        for y in range(y0, last+1):
            a = x0 + sa // dy01
            b = x0 + sb // dy02
            sa += dx01
            sb += dx02
            if a > b:
                a, b = b, a
            self.hline(a, y, b-a+1, *args, **kwargs)

        y = last

        sa = dx12 * (y - y1)
        sb = dx02 * (y - y0)
        while y <= y2:
            a = x1 + sa // dy12
            b = x0 + sb // dy02
            sa += dx12
            sb += dx02
            if a > b:
                a, b = b, a
            self.hline(a, y, b-a+1, *args, **kwargs)
            y += 1

    def round_rect(self, x0, y0, width, height, radius, *args, **kwargs):
        """Rectangle with rounded corners drawing function.
        This works like a regular rect though! if radius = 0
        Will draw the outline of a rextabgle with rounded corners with (x0,y0) at the top left"""
        # shift to correct for start point location
        x0 += radius
        y0 += radius

        # ensure that the radius will only ever half of the shortest side or less
        radius = int(min(radius, width / 2, height / 2))

        if radius:
            f = 1 - radius
            ddF_x = 1
            ddF_y = -2 * radius
            x = 0
            y = radius
            self.vline(
                x0 - radius, y0, height - 2 * radius + 1, *args, **kwargs
            )  # left
            self.vline(
                x0 + width - radius, y0, height - 2 * radius + 1, *args, **kwargs
            )  # right
            self.hline(
                x0, y0 + height - radius + 1, width - 2 * radius + 1, *args, **kwargs
            )  # bottom
            self.hline(x0, y0 - radius, width - 2 *
                       radius + 1, *args, **kwargs)  # top
            while x < y:
                if f >= 0:
                    y -= 1
                    ddF_y += 2
                    f += ddF_y
                x += 1
                ddF_x += 2
                f += ddF_x
                # angle notations are based on the unit circle and in diection of being drawn

                # top left
                self._pixel(x0 - y, y0 - x, *args, **kwargs)  # 180 to 135
                self._pixel(x0 - x, y0 - y, *args, **kwargs)  # 90 to 135
                # top right
                self._pixel(
                    x0 + x + width - 2 * radius, y0 - y, *args, **kwargs
                )  # 90 to 45
                self._pixel(
                    x0 + y + width - 2 * radius, y0 - x, *args, **kwargs
                )  # 0 to 45
                # bottom right
                self._pixel(
                    x0 + y + width - 2 * radius,
                    y0 + x + height - 2 * radius,
                    *args,
                    **kwargs,
                )  # 0 to 315
                self._pixel(
                    x0 + x + width - 2 * radius,
                    y0 + y + height - 2 * radius,
                    *args,
                    **kwargs,
                )  # 270 to 315
                # bottom left
                self._pixel(
                    x0 - x, y0 + y + height - 2 * radius, *args, **kwargs
                )  # 270 to 255
                self._pixel(
                    x0 - y, y0 + x + height - 2 * radius, *args, **kwargs
                )  # 180 to 225

    def fill_round_rect(self, x0, y0, width, height, radius, *args, **kwargs):
        """Filled circle drawing function.  Will draw a filled circule with
        center at x0, y0 and the specified radius."""
        # shift to correct for start point location
        x0 += radius
        y0 += radius

        # ensure that the radius will only ever half of the shortest side or less
        radius = int(min(radius, width / 2, height / 2))

        self.fill_rect(
            x0, y0 - radius, width - 2 * radius + 2, height + 2, *args, **kwargs
        )

        if radius:
            f = 1 - radius
            ddF_x = 1
            ddF_y = -2 * radius
            x = 0
            y = radius
            while x < y:
                if f >= 0:
                    y -= 1
                    ddF_y += 2
                    f += ddF_y
                x += 1
                ddF_x += 2
                f += ddF_x
                # part notation starts with 0 on left and 1 on right, and direction is noted
                # top left
                self.vline(
                    x0 - y, y0 - x, 2 * x + 1 + height - 2 * radius, *args, **kwargs
                )  # 0 to .25
                self.vline(
                    x0 - x, y0 - y, 2 * y + 1 + height - 2 * radius, *args, **kwargs
                )  # .5 to .25
                # top right
                self.vline(
                    x0 + x + width - 2 * radius,
                    y0 - y,
                    2 * y + 1 + height - 2 * radius,
                    *args,
                    **kwargs,
                )  # .5 to .75
                self.vline(
                    x0 + y + width - 2 * radius,
                    y0 - x,
                    2 * x + 1 + height - 2 * radius,
                    *args,
                    **kwargs,
                )  # 1 to .75

    def _place_char(self, x0, y0, char, size, *args, **kwargs):
        """A sub class used for placing a single character on the screen"""
        # pylint: disable=undefined-loop-variable
        arr = self.font[char]
        width = arr[0]
        height = arr[1]
        # extract the char section of the data
        data = arr[2:]
        for x in range(width):
            for y in range(height):
                bit = bool(data[x] & 2 ** y)
                # char pixel
                if bit:
                    self.fill_rect(
                        size * x + x0,
                        size * (height - y - 1) + y0,
                        size,
                        size,
                        *args,
                        **kwargs,
                    )
                # else background pixel
                else:
                    try:
                        self.fill_rect(
                            size * x + x0,
                            size * (height - y - 1) + y0,
                            size,
                            size,
                            *self.text_bkgnd_args,
                            **self.text_bkgnd_kwargs,
                        )
                    except TypeError:
                        pass
        del arr, width, height, data, x, y, x0, y0, char, size

    def _very_slow_text(self, x0, y0, string, size, *args, **kwargs):
        """a function to place text on the display.(temporary)
        to use special characters put "__" on either side of the desired characters.
        letter format:
        {'character_here' : bytearray(b',WIDTH,HEIGHT,right-most-data,
                                            more-bytes-here,left-most-data') ,}
        (replace the "," with backslashes!!)
        each byte:
                        | lower most bit(lowest on display)
                        V
                 x0110100
                  ^c
                  | top most bit (highest on display)"""

        x_roll = x0  # rolling x
        y_roll = y0  # rolling y

        # highest_height = 0#wrap
        sep_string = string.split("__")

        for chunk in sep_string:
            # print(chunk)
            try:
                self._place_char(x_roll, y_roll, chunk, size, *args, **kwargs)
                x_roll += size * self.font[chunk][0] + size
                # highest_height = max(highest_height, size*self.font[chunk][1] + 1) #wrap
            except KeyError:
                while chunk:
                    char = chunk[0]

                    # make sure something is sent even if not in font dict
                    try:
                        self._place_char(x_roll, y_roll, char,
                                         size, *args, **kwargs)
                    except KeyError:
                        self._place_char(
                            x_roll, y_roll, "?CHAR?", size, *args, **kwargs
                        )
                        char = "?CHAR?"

                    x_roll += size * self.font[char][0]

                    # gap between letters
                    try:
                        self.fill_rect(
                            x_roll,
                            y_roll,
                            size,
                            size * self.font[char][1],
                            *self.text_bkgnd_args,
                            **self.text_bkgnd_kwargs,
                        )
                    except TypeError:
                        pass

                    x_roll += size
                    # highest_height = max(highest_height, size*self.font[char][1] + 1) #wrap
                    chunk = chunk[1:]  # wrap
                    # if (x_roll >= self.width) or (chunk[0:2] == """\n"""): #wrap
                    # self._text(x0,y0+highest_height,"__".join(sep_string),size) #wrap
                    # print(highest_height) #wrap

    def set_text_background(self, *args, **kwargs):
        """A method to change the background color of text, input any and all color paramsself.
        run without any inputs to return to "clear" background
        """
        self.text_bkgnd_args = args
        self.text_bkgnd_kwargs = kwargs

    # pylint: enable=too-many-arguments
