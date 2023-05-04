from CircuitOS import AW9523


class VibroFire:
    def __init__(self, i2c, address: int = 0x59):
        self.vibro = False
        self.aw9523 = AW9523(i2c, address)
        self.vibro_pin = 8
        self.num_leds = 15
        self.fill_percentage = 0

    def begin(self):
        if not self.aw9523.begin():
            return False

        for i in range(16):
            if i == self.vibro_pin:
                self.aw9523.pin_mode(self.vibro_pin, AW9523.OUT)
                self.set_vibrating(self.vibro)
            else:
                self.aw9523.pin_mode(i, AW9523.LED)
                self.aw9523.dim(i, 0)

        return True

    def set_vibrating(self, on: bool):
        self.vibro = on
        self.aw9523.write(self.vibro_pin, not self.vibro)

    def set_led_fill(self, percentage: int):
        if self.fill_percentage == percentage:
            return

        self.fill_percentage = percentage
        fill = self.fill_percentage * self.num_leds * 255 // 100
        for i in range(self.num_leds):
            pixel_value = min(fill, 255)
            fp = (float(pixel_value) / 255.0) * 0.2
            fp = fp ** 2
            pixel_value = int(fp * 255.0)
            self.aw9523.dim(self.map_pin(i), pixel_value)
            if fill >= 255:
                fill -= 255
            elif fill > 0:
                fill = 0

    def set_led(self, index: int, value: int):
        fp = (float(value) / 255.0) * 0.2
        fp = fp ** 2
        value = int(fp * 255.0)
        self.aw9523.dim(self.map_pin(index), value)

    def map_pin(self, led: int):
        if led <= 3:
            return 15 - led
        elif led <= 11:
            return 11 - led
        else:
            return 23 - led
