# LCD testing object for PC
# uses double buffering
class LCD:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.buf1 = [[0 for x in range(self.width)] for y in range(self.height)]
        self.buf2 = [[0 for x in range(self.width)] for y in range(self.height)]

    def clear(self):
        for y in range(self.height):
            for x in range(self.width):
                self.buf1[y][x] = self.buf2[y][x] = 0

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

    def reset(self, x, y):
        if 0 <= x < self.width and 0 <= y < self.height:
            self.buf2[y][x] = 0

    def set(self, x, y):
        if 0 <= x < self.width and 0 <= y < self.height:
            self.buf2[y][x] = 1
