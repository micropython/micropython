from machine import Pin, ADC

class Slider:
    def __init__(self, pin: int, min: int = 0, max: int = 1024, ema_a: float = 0, reverse: bool = False):
        self.pin = Pin(pin, mode=Pin.IN)
        self.min = min
        self.max = max
        self.ema_a = ema_a
        self.reverse = reverse

        self.adc = ADC(self.pin)
        self.adc.atten(ADC.ATTN_11DB)
        self.adc.width(ADC.WIDTH_10BIT)

        initial_value = self.read()
        self.val = initial_value
        self.last_called_val = initial_value

        self._on_move = None

    def on_move(self, on_move: callable) -> None:
        self._on_move = on_move
        self.last_called_val = self.val

    def read(self) -> float:
        raw_value = self.adc.read()
        mapped_value = self.map_value(raw_value)
        if self.reverse:
            mapped_value = 100 - mapped_value
        return mapped_value

    def scan(self) -> None:
        mapped_value = self.read()

        if self.ema_a != 0:
            self.val = self.ema_a * mapped_value + (1 - self.ema_a) * self.val
        else:
            self.val = mapped_value

        if self._on_move and (self.last_called_val is None or round(self.last_called_val) != round(self.val)):
            self._on_move(round(self.val))
            self.last_called_val = self.val

    def map_value(self, x: int) -> float:
        x = max(self.min, min(self.max, x))
        return (x - self.min) * 100.0 / (self.max - self.min)
