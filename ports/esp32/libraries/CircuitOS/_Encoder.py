from machine import Pin


class Encoder:
    def __init__(self, pin_a: int, pin_b: int):
        self.pin_a = Pin(pin_a, mode=Pin.IN)
        self.pin_b = Pin(pin_b, mode=Pin.IN)

        self.prev_state = float('inf')
        self._on_move = None

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

    async def loop(self):
        from uasyncio import sleep_ms

        while True:
            self.scan()
            await sleep_ms(1)

    def on_move(self, listener):
        self._on_move = listener
