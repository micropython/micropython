# This file is part of the Micro Python project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2017 Glenn Ruben Bakke
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
E-paper EM027AS011.

Pin layout on pca10040 (nrf52):

EPAPER_PANEL_ON    13 (Arduino D2)
EPAPER_BORDER      14 (Arduino D3)
EPAPER_PWM         16 (Arduino D5)
EPAPER_RESET       17 (Arduino D6)
EPAPER_BUSY        18 (Arduino D7)
EPAPER_DISCHARGE   19 (Arduino D8)
EPAPER_TEMP_SENSOR 03 (Arduino A0)
EPAPER_CS          22 (Arduino D10)
EPAPER_DIN         23 (Arduino D11)
EPAPER_DOUT        24 (Arduino D12)
EPAPER_CLK         25 (Arduino D13)

Example usage on pca10040:
    
from epaper import Epaper
    
epd = Epaper()
epd.fill(0)
epd.text("Hello World!", 50, 50)
epd.show()
    
epd.refresh()
epd.refresh()
"""

import os
import time
import lcd_mono_fb

from machine import SPI, Pin, PWM

EPD_STATE_COMP   = const(0x1)
EPD_STATE_WHITE  = const(0x2)
EPD_STATE_INV    = const(0x3)
EPD_STATE_NORM   = const(0x4)  

class Epaper:
    def __init__(self, width=264, height=176, vertical=False):

        self.width = width
        self.height = height
        self.vertical = vertical
        self.framebuf = lcd_mono_fb.MonoFB(self.line_update, self.width, self.height, True)
        
        self.reset     = Pin("A17", mode=Pin.OUT, pull=Pin.PULL_UP)
        self.panel_on  = Pin("A13", mode=Pin.OUT, pull=Pin.PULL_UP)
        self.discharge = Pin("A19", mode=Pin.OUT, pull=Pin.PULL_UP)
        self.border    = Pin("A14", mode=Pin.OUT, pull=Pin.PULL_UP)
        self.busy      = Pin("A18", mode=Pin.IN,  pull=Pin.PULL_DISABLED)
        self.cs        = Pin("A22", mode=Pin.OUT, pull=Pin.PULL_UP)
        
        self.reset.low()
        self.panel_on.low()
        self.discharge.low()
        self.border.low()
        
        self.pwm = PWM(0, Pin("A16", mode=Pin.OUT), freq=PWM.FREQ_250KHZ, duty=50, period=2)
        
        # Min baudrate 4M, max 12M
        self.spi = SPI(0, baudrate=80000000)
        # self.spi.init(baudrate=8000000, phase=0, polarity=0)
        
    def line_update(self, o, line, new_bytes, old_bytes):
        if new_bytes:
            self._update_line(line, old_bytes, EPD_STATE_COMP)
            self._update_line(line, old_bytes, EPD_STATE_WHITE)
            self._update_line(line, new_bytes, EPD_STATE_INV)
            self._update_line(line, new_bytes, EPD_STATE_NORM)
        else:
            self._update_line(line, old_bytes, EPD_STATE_NORM)

    def clear(self):
        line_count = self.height;
        for i in range(0, line_count):
            self._update_line(i, None, EPD_STATE_COMP, 0xFF)
        
        time.sleep_ms(500)

        for i in range(0, line_count):
            self._update_line(i, None, EPD_STATE_WHITE, 0xAA)
            
        time.sleep_ms(500)

        for i in range(0, line_count):
            self._update_line(i, None, EPD_STATE_INV, 0xFF)
    
        time.sleep_ms(500)

        for i in range(0, line_count):
            self._update_line(i, None, EPD_STATE_NORM, 0xAA)
    
        time.sleep_ms(500)

    def init_display(self):
        
        self.pwm.init() # start the pwm

        print("sleep")
        time.sleep_ms(5)
        print("wakeup")
        self.panel_on.high()
        time.sleep_ms(10)

        self.reset.high()
        self.border.high()
        self.cs.high()
        
        time.sleep_ms(5)
        
        self.reset.low()
        
        time.sleep_ms(5)
        
        self.reset.high()

        time.sleep_ms(5)
        print("Wait for busy")
        self.wait_for_busy_release()
                
        time.sleep_us(10)
        # channel select
        self.write_data(bytearray([0x70, 0x01]))
        time.sleep_us(10)

        # CS
        self.write_data(bytearray([0x72, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00]))

        # DC/DC frequency
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x06]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0xff]))

        # high power mode osc
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x07]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x9d]))

        # disable ADC
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x08]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x00]))
    
        # Vcom level
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x09]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0xd0, 0x00]))
    
        # gate and source voltage levels
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x04]))
        
        # GS
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x00]))
    
        time.sleep_ms(5)
    
        # driver latch on
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x03]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x01]))
    
        # driver latch off
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x03]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x00]))
    
        time.sleep_ms(5)
    
        # charge pump positive voltage on
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x05]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x01]))
    
        # final delay before PWM off
        time.sleep_us(30)
    
        # stop PWM
        self.pwm.deinit()
    
        # charge pump negative voltage on
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x05]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x03]))
    
        time.sleep_us(30)
    
        # Vcom driver on
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x05]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x0f]))
    
        time.sleep_ms(30);
    
        # output enable to disable
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x02]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x24]))
        
    def wait_for_busy_release(self):
        # wait for COG to become ready
        while (self.busy.value() == 1): 
            pass
        
    def _update_line(self, line, data, state, fixed=None):
        time.sleep_us(10)
    
        self.write_data(bytearray([0x70, 0x04]))
        time.sleep_us(10)
    
        # gate source
        self.write_data(bytearray([0x72, 0x00]))
        time.sleep_us(10)
    
        self.write_data(bytearray([0x70, 0x0a]))
        time.sleep_us(10)
    
        self.cs.low()
        
        self.write_data_wait(bytearray([0x72]))
    
        bytes_per_line = self.width // 8;
    
        # even pixels
        if data:
            for i in range(bytes_per_line - 1, -1, -1):
            
                pixels = data[i] & 0xaa;
    
                if state == EPD_STATE_COMP:
                    # B -> W, W -> B (current image)
                    pixels = 0xaa | ((pixels ^ 0xaa) >> 1)
    
                elif state == EPD_STATE_WHITE:
                    # B -> N, W -> W (current image)
                    pixels = 0x55 + ((pixels ^ 0xaa) >> 1)
    
                elif state == EPD_STATE_INV:
                    # B -> N, W -> B (new image)
                    pixels = 0x55 | (pixels ^ 0xaa)
    
                elif state == EPD_STATE_NORM:
                    # B -> B, W -> W (new image)
                    pixels = 0xaa | (pixels >> 1)

                self.write_data_wait(bytearray([pixels]));
        else:
            self.write_data_wait(bytearray([fixed] * bytes_per_line));
    
        bytes_per_scan = 176 // 4;
        # scan line
        for i in range(0, bytes_per_scan):
            if (line // 4 == i):
                self.write_data_wait(bytearray([0xc0 >> (2 * (line & 0x03))]))
            else:
                self.write_data_wait(bytearray([0x00]))

        # odd pixels
        if data:
            for i in range (0, bytes_per_line):
                pixels = data[i] & 0x55
                
                if state == EPD_STATE_COMP:
                    pixels = 0xaa | (pixels ^ 0x55)
    
                elif state == EPD_STATE_WHITE:
                    pixels = 0x55 + (pixels ^ 0x55)
    
                elif state == EPD_STATE_INV:
                    pixels = 0x55 | ((pixels ^ 0x55) << 1)
    
                elif state == EPD_STATE_NORM:
                    pixels = 0xaa | pixels
    
                p1 = (pixels >> 6) & 0x03;
                p2 = (pixels >> 4) & 0x03;
                p3 = (pixels >> 2) & 0x03;
                p4 = (pixels >> 0) & 0x03;
                
                pixels = (p1 << 0) | (p2 << 2) | (p3 << 4) | (p4 << 6);
        
                self.write_data_wait(bytearray([pixels]))
        else:
            self.write_data_wait(bytearray([fixed] * bytes_per_line))
                
        # Complete line
        self.write_data_wait(bytearray([0x00]))
         
        self.cs.high()
        
        time.sleep_us(10)

    
        self.write_data(bytearray([0x70, 0x02]))
        time.sleep_us(10)
    
        self.write_data(bytearray([0x72, 0x2f]))
            
    def deinit_display(self):        
        # all display sizes
        self._update_line(0x7fff, None, EPD_STATE_NORM, 0x55)
        time.sleep_ms(25)
        self.border.low()
        time.sleep_ms(250)
        self.border.high()
     
        # latch reset turn on
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x03]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x01]))
        
        # output enable off
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x02]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x05]))
        
        # Vcom power off
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x05]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x0e]))
        
        # power off negative charge pump
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x05]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x02]))
        
        # discharge
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x04]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x0c]))
        time.sleep_us(120)
        
        # all charge pumps off
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x05]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x00]))
        
        # turn of osc
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x07]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x0d]))
        
        # discharge internal - 1
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x04]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x50]))
        time.sleep_us(40)
        
        # discharge internal - 2
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x04]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0xA0]))
        time.sleep_us(40)
        
        # discharge internal - 3
        time.sleep_us(10)
        self.write_data(bytearray([0x70, 0x04]))
        time.sleep_us(10)
        self.write_data(bytearray([0x72, 0x00]))
        
        # turn of power and all signals
        time.sleep_ms(10)
        self.reset.low()
        self.panel_on.low()
        self.border.low()

        # discharge pulse
        self.discharge.high()
        time.sleep_us(250)
        self.discharge.low()
        
        self.cs.high()
    
    def show(self):
        self.init_display()
        self.framebuf.show()
        self.deinit_display()

    def refresh(self):
        self.init_display()
        self.framebuf.refresh()
        self.deinit_display()
        
    def fill(self, col):
        self.framebuf.fill(col)
        
    def pixel(self, x, y, col):
        self.framebuf.pixel(x, y, col)

    def scroll(self, dx, dy):
        self.framebuf.scroll(dx, dy)

    def text(self, string, x, y, col=1):
        self.framebuf.text(string, x, y, col)
    
    def write_data_wait(self, buf):
        self.spi.write(buf)
        self.wait_for_busy_release()
        
    def write_data(self, buf):
        self.cs.low()
        self.spi.write(buf)
        self.cs.high()
