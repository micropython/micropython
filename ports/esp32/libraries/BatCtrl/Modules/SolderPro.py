from CircuitOS import AW9523
import time
from machine import I2C
from micropython import const


class SolderPro:
    class Buttons:
        P1_3 = const(8)
        P1_2 = const(9)
        P1_1 = const(10)
        P1_0 = const(11)

    class LEDs:
        P0_0 = const(0)
        P0_1 = const(1)
        P0_6 = const(6)
        P0_7 = const(7)
        P1_4 = const(12)
        P1_5 = const(13)

    class GPIOs:
        P0_0 = const(0)
        P0_1 = const(1)
        P0_2 = const(2)
        P0_3 = const(3)
        P0_4 = const(4)
        P0_5 = const(5)

    def __init__(self, i2c: I2C, address: int = 0x5B):
        self.aw9523 = AW9523(i2c, address)
        self.num_buttons = 4
        self.button_pins = [i for i in range(11, 7, -1)]
        self.led_pins = [0, 1, 6, 7, 12, 13, 14, 15]
        self.control_pins = [i for i in range(6)]

        self.state = [False] * self.num_buttons
        self.on_press = [None] * self.num_buttons
        self.on_release = [None] * self.num_buttons

    def begin(self):
        if not self.aw9523.begin():
            return False

        for pin in self.button_pins:
            self.aw9523.pin_mode(pin, AW9523.IN)

        for pin in self.led_pins:
            self.aw9523.pin_mode(pin, AW9523.LED)

        return True

    def set_pin(self, pin: int, value: bool):
        if pin not in self.control_pins:
            return
        self.aw9523.write(pin, value)

    def set_pin_mode(self, pin: int, mode: int):
        if pin not in self.control_pins:
            return
        self.aw9523.pin_mode(pin, mode)

    def get_button(self, i: int) -> bool:
        if i >= self.num_buttons:
            return False
        return self.state[i]

    def set_led(self, index: int, value: int):
        if index in self.led_pins:
            self.aw9523.dim(index, value)

    def on_press(self, i: int, callback):
        if i < self.num_buttons:
            self.on_press[i] = callback

    def on_release(self, i: int, callback):
        if i < self.num_buttons:
            self.on_release[i] = callback

    async def loop(self):
        while True:
            self.scan_buttons()
            await time.sleep_ms(1)

    def scan_buttons(self):
        state = []

        for pin in self.button_pins:
            state.append(not self.aw9523.read(pin))

        for i, s in enumerate(state):
            if s == 1:
                self.pressed(i)
            else:
                self.released(i)

    def pressed(self, i: int):
        if i < self.num_buttons:
            old = self.state[i]
            self.state[i] = True

            if old != self.state[i] and self.on_press[i] is not None:
                self.on_press[i]()

    def released(self, i: int):
        if i < self.num_buttons:
            old = self.state[i]
            self.state[i] = False

            if old != self.state[i] and self.on_release[i] is not None:
                self.on_release[i]()
