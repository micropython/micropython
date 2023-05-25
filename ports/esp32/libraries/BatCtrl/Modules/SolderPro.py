from CircuitOS import AW9523, Input
import time
from machine import I2C
from micropython import const


class SolderPro(Input):
    class Buttons:
        P1_3 = const(11)
        P1_2 = const(10)
        P1_1 = const(9)
        P1_0 = const(8)

    class LEDs:
        P0_0 = 0
        P0_1 = 1
        P0_6 = const(6)
        P0_7 = const(7)
        P1_4 = const(12)
        P1_5 = const(13)
        P1_6 = const(14)
        P1_7 = const(15)

    class GPIO:
        P0_0 = const(0)
        P0_1 = const(1)
        P0_2 = const(2)
        P0_3 = const(3)
        P0_4 = const(4)
        P0_5 = const(5)

    BtnToPin = {
        Buttons.P1_3: 0,
        Buttons.P1_2: 1,
        Buttons.P1_1: 2,
        Buttons.P1_0: 3,
    }

    def __init__(self, i2c: I2C, address: int = 0x5B):
        self.aw9523 = AW9523(i2c, address)
        super().__init__(16)

        self.num_buttons = 4
        self.button_pins = [i for i in range(8, 12)]
        self.led_pins = [0, 1, 6, 7, 12, 13, 14, 15]
        self.control_pins = [i for i in range(6)]

        self._on_press = [None] * self.num_buttons
        self._on_release = [None] * self.num_buttons

    def begin(self):
        if not self.aw9523.begin():
            return False

        for pin in self.button_pins:
            self.aw9523.pin_mode(pin, AW9523.IN)

        for pin in self.led_pins:
            self.aw9523.pin_mode(pin, AW9523.LED)

        return True

    def write_pin(self, pin: int, value: bool):
        if pin not in self.control_pins:
            return
        self.aw9523.write(pin, value)

    def read_pin(self, pin: int):
        if pin not in self.control_pins and pin not in self.button_pins:
            return
        return self.aw9523.read(pin)

    def set_pin_mode(self, pin: int, mode: int):
        if pin not in self.control_pins:
            return
        self.aw9523.pin_mode(pin, mode)

    def set_led(self, index: int, value: int):
        if index in self.led_pins:
            self.aw9523.dim(index, int((value*255)/100))

    def scan(self):
        state = []

        for i in range(16):
            state.append(not self.aw9523.read(i))

        for i, s in enumerate(state):
            if s == 1:
                self.pressed(i)
            else:
                self.released(i)
