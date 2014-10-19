# pyboard testing functions for CPython
import time

def delay(n):
    #time.sleep(float(n) / 1000)
    pass

rand_seed = 1
def rng():
    global rand_seed
    # for these choice of numbers, see P L'Ecuyer, "Tables of linear congruential generators of different sizes and good lattice structure"
    rand_seed = (rand_seed * 653276) % 8388593
    return rand_seed

# LCD testing object for PC
# uses double buffering
class LCD:
    def __init__(self, port):
        self.width = 128
        self.height = 32
        self.buf1 = [[0 for x in range(self.width)] for y in range(self.height)]
        self.buf2 = [[0 for x in range(self.width)] for y in range(self.height)]

    def light(self, value):
        pass

    def fill(self, value):
        for y in range(self.height):
            for x in range(self.width):
                self.buf1[y][x] = self.buf2[y][x] = value

    def show(self):
        print('') # blank line to separate frames
        for y in range(self.height):
            for x in range(self.width):
                self.buf1[y][x] = self.buf2[y][x]
        for y in range(self.height):
            row = ''.join(['*' if self.buf1[y][x] else ' ' for x in range(self.width)])
            print(row)

    def get(self, x, y):
        if 0 <= x < self.width and 0 <= y < self.height:
            return self.buf1[y][x]
        else:
            return 0

    def pixel(self, x, y, value):
        if 0 <= x < self.width and 0 <= y < self.height:
            self.buf2[y][x] = value
