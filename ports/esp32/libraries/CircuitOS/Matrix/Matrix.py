class Matrix:
    def __init__(self, output):
        self.output = output
        self.width = output.get_width()
        self.height = output.get_height()
        self.data = [[(0, 0, 0, 0)] * self.height for _ in range(self.width)]
        self.rotation = 0

    def begin(self):
        self.clear()
        self.push()

    def clear(self, color=(0, 0, 0, 0)):
        self.data = [[color] * self.height for _ in range(self.width)]

    def push(self):
        self.output.push(self.data)

    def draw_pixel(self, x, y, color):
        if self.rotation == 1:
            x, y = y, self.width - x - 1
        elif self.rotation == 2:
            x, y = self.width - x - 1, self.height - y - 1
        elif self.rotation == 3:
            x, y = self.height - y - 1, x

        if 0 <= x < self.width and 0 <= y < self.height:
            self.data[x][y] = color

    def draw_bitmap(self, x, y, width, height, data, color=(255, 255, 255, 255)):
        for i in range(width):
            for j in range(height):
                c = data[j * width + i]
                self.draw_pixel(x + i, y + j, tuple(int(a * c / 255) for a in color))

    def set_brightness(self, brightness):
        self.output.set_brightness(brightness)

    def get_brightness(self):
        return self.output.get_brightness()

    def set_rotation(self, rot):
        if 0 <= rot <= 3:
            self.rotation = rot

    def get_rotation(self):
        return self.rotation

    def get_width(self):
        return self.width

    def get_height(self):
        return self.height
