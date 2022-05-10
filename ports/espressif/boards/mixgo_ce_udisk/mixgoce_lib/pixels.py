"""
RGB

CircuitPython library for RGB - MixGoCE
=======================================================

Small Cabbage
20210721
"""


import time
import board
import neopixel

def wheel(pos):
    # Input a value 0 to 255 to get a color value.
    # The colours are a transition r - g - b - back to r.
    if pos < 0 or pos > 255:
        return (0, 0, 0)
    if pos < 85:
        return (255 - pos * 3, pos * 3, 0)
    if pos < 170:
        pos -= 85
        return (0, 255 - pos * 3, pos * 3)
    pos -= 170
    return (pos * 3, 0, 255 - pos * 3)


class RGB(object):
    def __init__(self,pin=board.IO8,num=4,flag="RGB"):
        self.pin = pin
        self.num = num
        self.pixels = neopixel.NeoPixel(pin, num, brightness=0.3, auto_write=False, pixel_order = flag)

    def show_all(self, R, G, B):
        color = (R, G, B)
        self.pixels.fill(color)

    def show_one(self, index, R, G, B):
        color = (R, G, B)
        self.pixels[index - 1] = color

    def write(self):
        self.pixels.show()
        
    def color_chase(self, R, G, B, wait):
        color = (R, G, B)
        for i in range(self.num):
            self.pixels[i] = color
            time.sleep(wait/1000)
            self.pixels.show()

    def rainbow_cycle(self, wait):
        for j in range(255):
            for i in range(self.num):
                rc_index = (i * 256 // self.num) + j
                self.pixels[i] = wheel(rc_index & 255)
            self.pixels.show()
            time.sleep(wait/1000/256)

    def change_mod(self,flag):
        import time
        if flag in ("RGB","GRB"):
            self.pixels.deinit()
            time.sleep(0.1)
            self.pixels = neopixel.NeoPixel(self.pin, self.num, brightness=0.3, auto_write=False, pixel_order = flag)

rgb = RGB()
rgb.show_all(0,0,0)
rgb.write()