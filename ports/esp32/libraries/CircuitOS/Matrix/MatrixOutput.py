from micropython import const


class MatrixOutput:
    def __init__(self, width, height):
        self.width = const(width)
        self.height = const(height)
        self.brightness = const(255)

    def init(self):
        raise NotImplementedError("init method should be implemented by subclass")

    def push(self, data):
        raise NotImplementedError("push method should be implemented by subclass")

    def get_width(self):
        return self.width

    def get_height(self):
        return self.height

    def get_brightness(self):
        return self.brightness

    def set_brightness(self, brightness):
        self.brightness = brightness
