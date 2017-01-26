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

'''
Usage:

from game2048 import Game
g = Game()
g.start()

'''

from machine import ADC
from machine import Pin, SPI
from display import SSD1305
import draw
import time

font_128 = [
    "  x   xx   xx",
    " xx  x  x x  x",
    "x x     x x  x",
    "  x    x   xx",
    "  x   x   x  x",
    "  x  x    x  x",
    " xxx xxxx  xx"
]

font_256 = [
    " xx  xxxx  xx",
    "x  x x    x  x",
    "   x xxx  x   ",
    "  x     x xxxx",
    " x      x x  x",
    "x       x x  x",
    "xxxx xxx   xx"
]

font_512 = [
    "xxxx    x   xx",
    "x      xx  x  x",
    "xxx   x x     x",
    "   x    x    x",
    "   x    x   x",
    "   x    x  x",
    "xxx    xxx xxxx"
]

font_1024 = [
    " xx  x   x  x x",
    "x x x x x x x x",
    "  x x x   x x x",
    "  x x x  x  xxx",
    "  x x x x     x",
    "  x x x x     x",
    "  x  x  xxx   x"
]

DIR_UP     = const(0x1)
DIR_DOWN   = const(0x2)
DIR_LEFT   = const(0x3)
DIR_RIGHT  = const(0x4)
DIR_CENTER = const(0x5)

class Game:
    def __init__(self):
        
#         # setup harware for nrf51822 / pca10028
#         cs = Pin("A14", mode=Pin.OUT, pull=Pin.PULL_UP)
#         reset = Pin("A13", mode=Pin.OUT, pull=Pin.PULL_UP)
#         dc = Pin("A12", mode=Pin.OUT, pull=Pin.PULL_UP)
#         spi = SPI(0, baudrate=8000000)
#         self.screen = SSD1305(128, 64, spi, cs, dc, reset)
#         self.x_adc = ADC(2)
#         self.y_adc = ADC(3)
#         self.adc_threshold = 205
        
        # setup harware for nrf52840 / pca10056
        cs = Pin("B3", mode=Pin.OUT, pull=Pin.PULL_UP)
        reset = Pin("B2", mode=Pin.OUT, pull=Pin.PULL_UP)
        dc = Pin("B1", mode=Pin.OUT, pull=Pin.PULL_UP)
        spi = SPI(0, baudrate=8000000)
        self.screen = SSD1305(128, 64, spi, cs, dc, reset)
        self.x_adc = ADC(1)
        self.y_adc = ADC(2)
        self.adc_threshold = 130

        # game setup
        self.grid = [[0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]
        self._rand = 1337
        self.not_game_over = True
        self.score = 0
        
        
    def move(self, dir):
        if (dir == DIR_UP):
            # float up
            for col in range(0, 4):
                for row1 in range(1, 5):
                    for row0 in range(1, row1):
                        if self.grid[col][row0 - 1] == 0:
                            if self.grid[col][row0]:
                                self.grid[col][row0 - 1] = self.grid[col][row0]
                                self.grid[col][row0] = 0
            # collapse
            for col in range(0, 4):
                for row0 in range(0, 3):
                    if self.grid[col][row0] == self.grid[col][row0 + 1]:
                        self.grid[col][row0] += self.grid[col][row0]
                        self.score += self.grid[col][row0]
                        self.grid[col][row0 + 1] = 0                     
            # float up
            for col in range(0, 4):
                for row1 in range(1, 5):
                    for row0 in range(1, row1):
                        if self.grid[col][row0 - 1] == 0:
                            if self.grid[col][row0]:
                                self.grid[col][row0 - 1] = self.grid[col][row0]
                                self.grid[col][row0] = 0
        elif (dir == DIR_DOWN):
            # float down
            for col in range(0, 4):
                for row1 in range(1, 5):
                    for row0 in range(row1, 4):
                        if self.grid[col][row0] == 0:
                            if self.grid[col][row0 - 1]:
                                self.grid[col][row0] = self.grid[col][row0 - 1]
                                self.grid[col][row0 - 1] = 0 
            # collapse
            for col in range(0, 4):
                for row0 in range(1, 4):
                    if self.grid[col][row0] == self.grid[col][row0 - 1]:
                        self.grid[col][row0] += self.grid[col][row0]
                        self.score += self.grid[col][row0]
                        self.grid[col][row0 - 1] = 0                      
            # float down
            for col in range(0, 4):
                for row1 in range(1, 5):
                    for row0 in range(row1, 4):
                        if self.grid[col][row0] == 0:
                            if self.grid[col][row0 - 1]:
                                self.grid[col][row0] = self.grid[col][row0 - 1]
                                self.grid[col][row0 - 1] = 0 
        elif (dir == DIR_LEFT):
            # float left
            for row in range(0, 4):
                for col1 in range(1, 5):
                    for col0 in range(1, col1):
                        if self.grid[col0 - 1][row] == 0:
                            if self.grid[col0][row]:
                                self.grid[col0 - 1][row] = self.grid[col0][row]
                                self.grid[col0][row] = 0                        
            # collapse
            for row in range(0, 4):
                for col0 in range(0, 3):
                    if self.grid[col0][row] == self.grid[col0 + 1][row]:
                        self.grid[col0][row] += self.grid[col0][row]
                        self.score += self.grid[col0][row]
                        self.grid[col0 + 1][row] = 0                     
            # float left
            for row in range(0, 4):
                for col1 in range(1, 5):
                    for col0 in range(1, col1):
                        if self.grid[col0 - 1][row] == 0:
                            if self.grid[col0][row]:
                                self.grid[col0 - 1][row] = self.grid[col0][row]
                                self.grid[col0][row] = 0
        elif (dir == DIR_RIGHT):
            # float right
            for row in range(0, 4):
                for col1 in range(1, 5):
                    for col0 in range(col1, 4):
                        if self.grid[col0][row] == 0:
                            if self.grid[col0 - 1][row]:
                                self.grid[col0][row] = self.grid[col0 - 1][row]
                                self.grid[col0 - 1][row] = 0 
            # collapse
            for row in range(0, 4):
                for col0 in range(1, 4):
                    if self.grid[col0][row] == self.grid[col0 - 1][row]:
                        self.grid[col0][row] += self.grid[col0][row]
                        self.score += self.grid[col0][row]
                        self.grid[col0 - 1][row] = 0                     
            # float right
            for row in range(0, 4):
                for col1 in range(1, 5):
                    for col0 in range(col1, 4):
                        if self.grid[col0][row] == 0:
                            if self.grid[col0 - 1][row]:
                                self.grid[col0][row] = self.grid[col0 - 1][row]
                                self.grid[col0 - 1][row] = 0 
    def set_val(self, x, y, val):
        self.grid[x][y] = val
    def get_val(self, x, y):
        return self.grid[x][y]
    def draw_high_number(self, font, x, y):
        for rel_y, line in enumerate(font):
            for rel_x in range(0, len(line)):
                if (line[rel_x] == 'x'):
                    self.screen.pixel(x + rel_x, y + rel_y, 1)
    def draw_tile(self, x, y):
        val = self.get_val(x, y)
        if val != 0:
            x0 = x * 16
            y0 = y * 16
            if (val < 16):
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                draw.text(self.screen, str(val), x0 + 4, y0 + 4, 1)        
            elif val == 16:
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                draw.text(self.screen, "1", x0 + 1, y0 + 4, 1)
                draw.text(self.screen, "6", x0 + 7, y0 + 4, 1)
            elif val == 32:
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                draw.text(self.screen, "3", x0 + 1, y0 + 4, 1)
                draw.text(self.screen, "2", x0 + 7, y0 + 4, 1)
            elif val == 64:
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                draw.text(self.screen, "6", x0 + 1, y0 + 4, 1)
                draw.text(self.screen, "4", x0 + 7, y0 + 4, 1)
            elif val == 128:
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                self.draw_high_number(font_128, x0 + 1, y0 + 4)
            elif val == 256:
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                self.draw_high_number(font_256, x0 + 1, y0 + 4)
            elif val == 512:
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                self.draw_high_number(font_512, x0 + 1, y0 + 4)
            elif val == 1024:
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                self.draw_high_number(font_1024, x0 + 1, y0 + 4)
            elif val == 2048:
                draw.rectangle(self.screen, x0, y0, x0 + 15, y0 + 15, 1)
                self.draw_high_number(font_2048, x0 + 1, y0 + 4)
    def read_stick_x(self):
        return self.x_adc.value()
    def read_stick_y(self):
        return self.y_adc.value()
    def wait_for_move(self):
        x_val = self.read_stick_x()
        y_val = self.read_stick_y()
        if  x_val > self.adc_threshold + 15: 
            return DIR_RIGHT
        elif x_val < self.adc_threshold - 15:
            return DIR_LEFT
        if y_val > self.adc_threshold + 15:
            return DIR_UP
        elif y_val < self.adc_threshold - 15:
            return DIR_DOWN 
        return DIR_CENTER   
    def get_free_tiles(self):
        list = []
        for x in range(0, 4):
            for y in range(0, 4):
                if self.get_val(x, y) == 0:
                    list.append((x, y))
        return list
    def draw_all_tiles(self):
        for x in range(0, 4):
            for y in range(0, 4):
                self.draw_tile(x, y)
    def rand(self, mod=0):
        self._rand = ((214013*self._rand+2531011)>>16)&0x7FFF
        if mod:
            return self._rand % mod
        else:
            return self._rand
    def add_random_tile(self):
        free_tiles = self.get_free_tiles()
        if free_tiles:
            x,y = free_tiles[self.rand(len(free_tiles))]    
            new_val = 2 << self.rand(2)
            self.set_val(x, y, new_val)
            return True
        else:
            return False
    def draw_score(self):
        draw.text(self.screen, "Score:", 70, 10, 1)
        draw.text(self.screen, str(self.score), 70, 20, 1)
    def clear_grid(self):
        # Because the the double array does not initialize correctly 
        # in micropython, we do an explicit clear
        for i in range(0, 4):
            for j in range(0, 4):
                self.grid[i][j] = 0
    def start(self):
        self.screen.fill(0)
        draw.text(self.screen, "Touch the stick", 10, 20, 1)
        draw.text(self.screen, " to start the", 10, 30, 1)
        draw.text(self.screen, "   game! =)", 10, 40, 1)
        self.screen.show()
        in_center = False
        x_move = True
        y_move = True
        wait_for_center = False
        self.clear_grid()
        while self.not_game_over:
            move_dir = self.wait_for_move()
            if move_dir != DIR_CENTER and wait_for_center == False:
                self.move(move_dir)                
                self.screen.fill(0)
                res = self.add_random_tile()
                if not res:
                    self.not_game_over = False
                else:
                    self.draw_all_tiles()
                    self.draw_score()
                    self.screen.show()
                    wait_for_center = True
            if move_dir == DIR_CENTER:
                wait_for_center = False
        self.screen.fill(0)
        draw.text(self.screen, "Game Over!", 10, 20, 1)        
        draw.text(self.screen, "Score:", 10, 30, 1)
        draw.text(self.screen, str(self.score), 10, 40, 1)
        self.screen.show()