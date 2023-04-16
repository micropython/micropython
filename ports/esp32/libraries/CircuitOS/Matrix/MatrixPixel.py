class MatrixPixel:
    RED = (255, 0, 0, 255)
    GREEN = (0, 255, 0, 255)
    BLUE = (0, 0, 255, 255)
    YELLOW = (255, 255, 0, 255)
    CYAN = (0, 255, 255, 255)
    MAGENTA = (255, 0, 255, 255)
    WHITE = (255, 255, 255, 255)
    BLACK = (0, 0, 0, 255)
    OFF = (0, 0, 0, 0)


class MatrixPixelData:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.data = [[(0, 0, 0, 0)] * height for _ in range(width)]

    def __eq__(self, other):
        return self.data == other.data

    def __ne__(self, other):
        return self.data != other.data

    def set(self, x, y, pixel):
        if x < self.width and y < self.height:
            self.data[x][y] = pixel

    def get(self, x, y):
        if x < self.width and y < self.height:
            return self.data[x][y]
        else:
            return None

    def clear(self, color=(0, 0, 0, 0)):
        for x in range(self.width):
            self.data[x] = [color] * self.height

    def get_width(self):
        return self.width

    def get_height(self):
        return self.height

    def __getitem__(self, x):
        return self.data[x] if x < self.width else None
