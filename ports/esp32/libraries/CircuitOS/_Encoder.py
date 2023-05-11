from machine import Pin


class Encoder:
    def __init__(self, pin_a: int, pin_b: int):
        self.pin_a = Pin(pin_a, mode=Pin.IN)
        self.pin_b = Pin(pin_b, mode=Pin.IN)

        self.prev_state = float('inf')
        self._on_move = None
        self._on_move_l = None
        self._on_move_r = None

    def scan(self):
        val = 0

        state_a = self.pin_a.value()
        state_b = self.pin_b.value()

        if state_a != self.prev_state and self.prev_state != float('inf'):
            if state_b != state_a:
                val = 1
            else:
                val = -1

        self.prev_state = state_a

        if val != 0 and self._on_move is not None:
            self._on_move(val)

        if val == -1 and self._on_move_l is not None:
            self._on_move_l()

        if val == 1 and self._on_move_r is not None:
            self._on_move_r()

    async def loop(self):
        from uasyncio import sleep_ms

        while True:
            self.scan()
            await sleep_ms(1)

    def on_move(self, listener: callable):
        self._on_move = listener

    def on_move_left(self, listener: callable):
        self._on_move_l = listener

    def on_move_right(self, listener: callable):
        self._on_move_r = listener


class Encoders:

    def __init__(self, enc: [Encoder]):
        self.enc = enc

    def on_move(self, i: int, listener: callable):
        if i < 0 or i >= len(self.enc):
            return
        self.enc[i].on_move(listener)

    def on_move_left(self, i: int, listener: callable):
        if i < 0 or i >= len(self.enc):
            return
        self.enc[i].on_move_left(listener)

    def on_move_right(self, i: int, listener: callable):
        if i < 0 or i >= len(self.enc):
            return
        self.enc[i].on_move_right(listener)

    def scan(self):
        for enc in self.enc:
            enc.scan()
