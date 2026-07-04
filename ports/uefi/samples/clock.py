# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
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

# clock.py — a ticking analogue clock drawn with the GOP framebuffer.
#
# Run with a display:  make run-gfx
# Then at the REPL:     exec(open('/samples/clock.py').read())
#
# Uses uefi.Display, which hands you a MicroPython framebuf.FrameBuffer to draw on.

import uefi
import time
import math
import select
import sys
from uefi.display import rgb565

d = uefi.Display()
fb = d.fb
W, H = d.width, d.height
cx, cy = W // 2, H // 2
R = min(cx, cy) - 20

print("display: %dx%d, modes:" % (W, H))
for m, mw, mh in d.modes():
    print("  [%d] %dx%d" % (m, mw, mh))

WHITE = rgb565(255, 255, 255)
GREY = rgb565(90, 90, 90)
RED = rgb565(255, 60, 60)
BLACK = 0


def hand(length, angle, color):
    fb.line(cx, cy, cx + int(length * math.sin(angle)), cy - int(length * math.cos(angle)), color)


def draw():
    fb.fill(BLACK)
    fb.ellipse(cx, cy, R, R, WHITE)  # face
    for i in range(12):  # hour ticks
        a = math.pi * i / 6
        fb.line(
            cx + int((R - 4) * math.sin(a)),
            cy - int((R - 4) * math.cos(a)),
            cx + int((R - 22) * math.sin(a)),
            cy - int((R - 22) * math.cos(a)),
            GREY,
        )
    t = time.localtime()
    h, m, s = t[3] % 12, t[4], t[5]
    hand(R * 0.5, math.pi * (h + m / 60) / 6, WHITE)  # hour hand
    hand(R * 0.78, math.pi * m / 30, WHITE)  # minute hand
    hand(R * 0.9, math.pi * s / 30, RED)  # second hand
    fb.ellipse(cx, cy, 5, 5, WHITE, True)  # centre hub
    d.show()


print("analogue clock — press a key to stop")
poll = select.poll()
poll.register(sys.stdin, select.POLLIN)
last = -1
while True:
    now = time.localtime()[5]
    if now != last:  # redraw only when the second changes
        last = now
        draw()
    if poll.poll(50):  # sleep up to 50 ms on the WFE; any key exits
        sys.stdin.read(1)
        break
print("stopped")
