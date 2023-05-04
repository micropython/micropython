from machine import I2C
import time


class AW9523:
    ADDR = 0x58

    IN, OUT, LED = range(3)
    IMAX, IMAX_3Q, IMAX_2Q, IMAX_1Q = range(4)
    _dimmap = [4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3, 12, 13, 14, 15]

    def __init__(self, i2c: I2C, address: int = ADDR):
        self._i2c = i2c
        self._addr = address
        self._regs = {
            'conf': 0,
            'dir': [0, 0],
            'output': [0, 0],
            'intr': [0, 0],
            'mode': [0xff, 0xff],
            'dim': [0] * 16
        }

    def _write_reg(self, reg: int, data: bytearray):
        buf = bytearray([reg]) + data
        self._i2c.writeto(self._addr, buf)

    def _read_reg(self, reg) -> int:
        self._i2c.writeto(self._addr, bytearray([reg]))
        return self._i2c.readfrom(self._addr, 1)[0]

    def begin(self) -> bool:
        try:
            self.reset()
            return self._read_reg(0x10) == 0x23
        except OSError:
            return False

    def reset(self):
        self._regs = {
            'conf': 0,
            'dir': [0, 0],
            'output': [0, 0],
            'intr': [0, 0],
            'mode': [0xff, 0xff],
            'dim': [0] * 16
        }
        self._write_reg(0x7F, bytearray([0x00]))
        time.sleep_us(50)

    def pin_mode(self, pin: int, mode: int):
        if pin >= 16:
            return

        it = 0 if pin <= 7 else 1
        mask = 1 << (pin if pin <= 7 else pin - 8)
        reg_dir = 0x04 + it
        reg_mode = 0x12 + it

        if mode == self.LED:
            self._regs['mode'][it] &= ~mask
            self._write_reg(reg_mode, bytearray([self._regs['mode'][it]]))
        else:
            self._regs['mode'][it] |= mask
            self._write_reg(reg_mode, bytearray([self._regs['mode'][it]]))

            if mode == self.OUT:
                self._regs['dir'][it] &= ~mask
            elif mode == self.IN:
                self._regs['dir'][it] |= mask
            else:
                print("Incorrect mode parameter, use AW9523.IN, AW9523.OUT, or AW9523.LED")
                return

            self._write_reg(reg_dir, bytearray([self._regs['dir'][it]]))

    def read(self, pin: int) -> bool:
        if pin >= 16:
            return False

        reg = (0x00 if pin <= 7 else 0x01)
        return (self._read_reg(reg) & (1 << (pin if pin <= 7 else pin - 8))) != 0

    def write(self, pin: int, state: bool):
        if pin >= 16:
            return

        it = 0 if pin <= 7 else 1
        mask = 1 << (pin if pin <= 7 else pin - 8)
        reg = 0x02 + it
        if state:
            self._regs['output'][it] |= mask
        else:
            self._regs['output'][it] &= ~mask

        self._write_reg(reg, bytearray([self._regs['output'][it]]))

    def dim(self, pin: int, factor: int):
        if pin >= 16:
            return

        pin = AW9523._dimmap[pin]
        self._regs['dim'][pin] = factor
        self._write_reg(0x20 + pin, bytearray([factor]))

    def set_interrupt(self, pin: int, enabled: bool):
        if pin >= 16:
            return

        it = 0 if pin <= 7 else 1
        mask = 1 << (pin if pin <= 7 else pin - 8)
        reg = 0x06 + it

        if enabled:
            self._regs['intr'][it] |= mask
        else:
            self._regs['intr'][it] &= ~mask

        self._write_reg(reg, bytearray([self._regs['intr'][it]]))

    def set_current_limit(self, limit: int):
        if limit > 3:
            return

        mask = 0b00000011
        self._regs['conf'] = (self._regs['conf'] & ~mask) | (limit & mask)
        self._write_reg(0x11, bytearray([self._regs['conf'] & 0b00010011]))
