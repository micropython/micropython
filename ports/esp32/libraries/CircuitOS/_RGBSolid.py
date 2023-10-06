from machine import Pin, Signal
from micropython import const
from CircuitOS import PCA95XX


class RGBSolid:
    class Color:
        Off = const(0)
        Red = const(1)
        Green = const(2)
        Blue = const(3)
        Yellow = const(4)
        Magenta = const(5)
        Cyan = const(6)
        White = const(7)

    rgbMap = [
        const(0b000),
        const(0b100),
        const(0b010),
        const(0b001),
        const(0b110),
        const(0b101),
        const(0b011),
        const(0b111)
    ]

    def __init__(self, pin_r: int, pin_g: int, pin_b: int):
        self.color = self.Color.Off
        self.pin_r = pin_r
        self.pin_g = pin_g
        self.pin_b = pin_b

    def set_color(self, color: int):
        self.color = color
        self.push()

    def get_color(self) -> int:
        return self.color

    def push(self):
        pass


class RGBSolidGPIO(RGBSolid):

    def __init__(self, pin_r: int, pin_g: int, pin_b: int):
        super().__init__(pin_r, pin_g, pin_b)
        self.pin_r = Signal(Pin(pin_r, mode=Pin.OUT, value=False))
        self.pin_g = Signal(Pin(pin_g, mode=Pin.OUT, value=False))
        self.pin_b = Signal(Pin(pin_b, mode=Pin.OUT, value=False))
        self.set_color(self.Color.Off)

    def push(self):
        color_code = self.rgbMap[self.color]
        self.pin_r.on() if color_code & 0b100 else self.pin_r.off()
        self.pin_g.on() if color_code & 0b010 else self.pin_g.off()
        self.pin_b.on() if color_code & 0b001 else self.pin_b.off()


class RGBSolidExpander(RGBSolid):

    def __init__(self, pin_r: int, pin_g: int, pin_b: int, expander: PCA95XX):
        super().__init__(pin_r, pin_g, pin_b)
        self.expander = expander
        self.expander.pin_mode(pin_r, Pin.OUT)
        self.expander.pin_mode(pin_g, Pin.OUT)
        self.expander.pin_mode(pin_b, Pin.OUT)
        self.set_color(self.Color.Off)

    def push(self):
        color_code = self.rgbMap[self.color]
        self.expander.pin_write(self.pin_r, bool(color_code & 0b100))
        self.expander.pin_write(self.pin_g, bool(color_code & 0b010))
        self.expander.pin_write(self.pin_b, bool(color_code & 0b001))
