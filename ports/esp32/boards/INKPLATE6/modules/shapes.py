# Copyright © 2020 by Thorsten von Eicken.

# Shapes is intended to be a mix-in that adds methods to a class that is derived from the
# MicroPython framebuf.FrameBuffer class. It adds methods to draw circles, rounded rectangles
# etc. All the code is cobbled together from elsewhere, please refer to the embedded copyright
# notices for the exact provenance.
# Methods in Shapes call self.pixel, self.line, self.hline, and self.vline, so these must be
# provided by the class that Shapes is mixed into.
class Shapes:

    # __mix_me_in adds the methods of this class to another class.  Typical usage is to put
    # something like Shapes.__mix_me_in(MyClass) after the class definition.
    @classmethod
    def __mix_me_in(cls, target):
        for func in dir(cls):
            if not callable(getattr(cls, func)) or func.startswith("__"):
                continue
            setattr(target, func, getattr(cls, func))

    # The following shapes are adapted from https://github.com/peterhinch/micropython-epaper
    # Copyright 2015 Peter Hinch
    #
    # Licensed under the Apache License, Version 2.0 (the "License") you may not use this
    # file except in compliance with the License.  You may obtain a copy of the License at:
    #
    #   http:#www.apache.org/licenses/LICENSE-2.0
    #
    # Unless required by applicable law or agreed to in writing, software distributed under
    # the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
    # KIND, either express or implied.  See the License for the specific language governing
    # permissions and limitations under the License.
    # Code translated and developed from https://developer.mbed.org/users/dreschpe/code/EaEpaper/

    def circle(self, x0, y0, r, color):  # Single pixel circle
        x = -r
        y = 0
        err = 2 - 2 * r
        while x <= 0:
            self.pixel(x0 - x, y0 + y, color)
            self.pixel(x0 + x, y0 + y, color)
            self.pixel(x0 + x, y0 - y, color)
            self.pixel(x0 - x, y0 - y, color)
            e2 = err
            if e2 <= y:
                y += 1
                err += y * 2 + 1
                if -x == y and e2 <= x:
                    e2 = 0
            if e2 > x:
                x += 1
                err += x * 2 + 1

    def fill_circle(self, x0, y0, r, color):  # Draw filled circle
        x0, y0, r = int(x0), int(y0), int(r)
        x = -r
        y = 0
        err = 2 - 2 * r
        while x <= 0:
            self.vline(x0 - x, y0 - y, 2*y+1, color)
            self.vline(x0 + x, y0 - y, 2*y+1, color)
            e2 = err
            if e2 <= y:
                y += 1
                err += y * 2 + 1
                if -x == y and e2 <= x:
                    e2 = 0
            if e2 > x:
                x += 1
                err += x * 2 + 1

    # The following shapes are adapted from https://github.com/adafruit/Adafruit_CircuitPython_GFX
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

    def triangle(self, x0, y0, x1, y1, x2, y2, color):
        """Triangle drawing function.  Will draw a single pixel wide triangle
        around the points (x0, y0), (x1, y1), and (x2, y2)."""
        self.line(x0, y0, x1, y1, color)
        self.line(x1, y1, x2, y2, color)
        self.line(x2, y2, x0, y0, color)

    def fill_triangle(self, x0, y0, x1, y1, x2, y2, color):
        """Filled triangle drawing function.  Will draw a filled triangle around
        the points (x0, y0), (x1, y1), and (x2, y2)."""
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
            self.hline(a, y0, b - a + 1, color)
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
            last = y1 - 1
        for y in range(y0, last + 1):
            a = x0 + sa // dy01
            b = x0 + sb // dy02
            sa += dx01
            sb += dx02
            if a > b:
                a, b = b, a
            self.hline(a, y, b - a + 1, color)
        sa = dx12 * (y - y1)
        sb = dx02 * (y - y0)
        while y <= y2:
            a = x1 + sa // dy12
            b = x0 + sb // dy02
            sa += dx12
            sb += dx02
            if a > b:
                a, b = b, a
            self.hline(a, y, b - a + 1, color)
            y += 1

    def round_rect(self, x0, y0, width, height, radius, color):
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
            self.vline(x0 - radius, y0, height - 2 * radius + 1, color)  # left
            self.vline(x0 + width - radius, y0, height - 2 * radius + 1, color)  # right
            self.hline(x0, y0 + height - radius + 1, width - 2 * radius + 1, color)  # bottom
            self.hline(x0, y0 - radius, width - 2 * radius + 1, color)  # top
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
                self.pixel(x0 - y, y0 - x, color)  # 180 to 135
                self.pixel(x0 - x, y0 - y, color)  # 90 to 135
                # top right
                self.pixel(x0 + x + width - 2 * radius, y0 - y, color)  # 90 to 45
                self.pixel(x0 + y + width - 2 * radius, y0 - x, color)  # 0 to 45
                # bottom right
                self.pixel(
                    x0 + y + width - 2 * radius, y0 + x + height - 2 * radius, color
                )  # 0 to 315
                self.pixel(
                    x0 + x + width - 2 * radius, y0 + y + height - 2 * radius, color
                )  # 270 to 315
                # bottom left
                self.pixel(x0 - x, y0 + y + height - 2 * radius, color)  # 270 to 255
                self.pixel(x0 - y, y0 + x + height - 2 * radius, color)  # 180 to 225

    def fill_round_rect(self, x0, y0, width, height, radius, color):
        """Filled circle drawing function.  Will draw a filled circule with
        center at x0, y0 and the specified radius."""
        # shift to correct for start point location
        x0 += radius
        y0 += radius

        # ensure that the radius will only ever half of the shortest side or less
        radius = int(min(radius, width / 2, height / 2))

        self.fill_rect(x0, y0 - radius, width - 2 * radius + 2, height + 2, color)

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
                self.vline(x0 - y, y0 - x, 2 * x + 1 + height - 2 * radius, color)  # 0 to .25
                self.vline(x0 - x, y0 - y, 2 * y + 1 + height - 2 * radius, color)  # .5 to .25
                # top right
                self.vline(
                    x0 + x + width - 2 * radius, y0 - y, 2 * y + 1 + height - 2 * radius, color
                )  # .5 to .75
                self.vline(
                    x0 + y + width - 2 * radius, y0 - x, 2 * x + 1 + height - 2 * radius, color
                )  # 1 to .75
