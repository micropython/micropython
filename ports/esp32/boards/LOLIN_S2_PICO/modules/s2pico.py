# LOLIN S2 PICO MicroPython Helper Library


from micropython import const
from machine import Pin, SPI, ADC, I2C
import machine, time
import ssd1306

import network


# Pin Assignments

# SPI
SPI_MOSI = const(35)
SPI_MISO = const(36)
SPI_CLK = const(37)

# I2C
I2C_SDA = const(8)
I2C_SCL = const(9)

# DAC
DAC1 = const(17)
DAC2 = const(18)

# LED
LED = const(10)

# OLED
OLED_RST = const(18)

#BUTTON
BUTTON = const(0)




class led:
    def __init__(self):
        self._led = Pin(LED, Pin.OUT, value=1)
    def on(self):
        self._led.value(0)
    def off(self):
        self._led.value(1)

class button:
    def __init__(self):
        self._btn = Pin(BUTTON, Pin.IN, Pin.PULL_UP)

    def get_button_status(self):
        return self._btn.value()

    def set_button_callback(self, cb):
        self._btn.irq(trigger=Pin.IRQ_FALLING, handler=cb)

class oled(ssd1306.SSD1306_I2C):
    def __init__(self):
        self.reset()
        
        # super().__init__(128,32,I2C(0))

    def reset(self):

        Pin(OLED_RST, Pin.OUT).value(1)
        time.sleep_ms(1) 
        Pin(OLED_RST, Pin.OUT).value(0)
        time.sleep_ms(10) 
        Pin(OLED_RST, Pin.OUT).value(1)

        super().__init__(128,32,I2C(0))

    def test(self):
        

        self.fill(0)
        self.fill_rect(0, 0, 32, 32, 1)
        self.fill_rect(2, 2, 28, 28, 0)
        self.vline(9, 8, 22, 1)
        self.vline(16, 2, 22, 1)
        self.vline(23, 8, 22, 1)
        self.fill_rect(26, 24, 2, 4, 1)
        self.text('MicroPython', 40, 0, 1)
        self.text('SSD1306', 40, 12, 1)
        self.text('OLED 128x32', 40, 24, 1)
        self.show()
    
    def display_wifi(self):

        self.fill(0)
        self.text("Scan...", 0, 0, 1)
        self.show()
        
        sta_if = network.WLAN(network.STA_IF)
        sta_if.active(True)
        _wifi=sta_if.scan()

        self.fill(0)
        self.text(str(len(_wifi))+' Networks', 0, 0, 1)
        self.text(str(_wifi[0][3])+"  -  "+(_wifi[0][0]).decode("utf-8") , 0, 12, 1)
        self.text(str(_wifi[1][3])+"  -  "+(_wifi[1][0]).decode("utf-8") , 0, 24, 1)

        self.show()
    





