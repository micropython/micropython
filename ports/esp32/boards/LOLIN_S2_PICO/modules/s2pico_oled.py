from time import sleep_ms
from ssd1306 import SSD1306_I2C
import network


class OLED(SSD1306_I2C):
    def __init__(self, i2c, reset):
        reset.init(reset.OUT, value=1)
        self._reset = reset
        self.reset(False)
        super().__init__(128, 32, i2c)

    def reset(self, reinit=True):
        self._reset(1)
        sleep_ms(1)
        self._reset(0)
        sleep_ms(10)
        self._reset(1)
        if reinit:
            self.init_display()

    def test(self):
        self.fill(0)
        self.fill_rect(0, 0, 32, 32, 1)
        self.fill_rect(2, 2, 28, 28, 0)
        self.vline(9, 8, 22, 1)
        self.vline(16, 2, 22, 1)
        self.vline(23, 8, 22, 1)
        self.fill_rect(26, 24, 2, 4, 1)
        self.text("MicroPython", 40, 0, 1)
        self.text("SSD1306", 40, 12, 1)
        self.text("OLED 128x32", 40, 24, 1)
        self.show()

    def display_wifi(self):
        self.fill(0)
        self.text("Scan...", 0, 0, 1)
        self.show()

        sta_if = network.WLAN(network.WLAN.IF_STA)
        sta_if.active(True)
        _wifi = sta_if.scan()

        self.fill(0)
        self.text(str(len(_wifi)) + " Networks", 0, 0, 1)
        self.text(str(_wifi[0][3]) + " " + (_wifi[0][0]).decode("utf-8"), 0, 12, 1)
        self.text(str(_wifi[1][3]) + " " + (_wifi[1][0]).decode("utf-8"), 0, 24, 1)
        self.show()
