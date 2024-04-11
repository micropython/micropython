

from machine import UART,WDT,Pin,I2C,PWM
import neopixel
import time
from observer_analog import AnalogPins
from observer_vesc import VESC
import ble_simple_peripheral
import neopixel_demo
from log import Log
import micropython_dotstar as dotstar
from machine import Pin, SPI
from lcd_i2c import LCD
from ssd1306 import SSD1306_I2C
from pyvesc.VESC.messages import GetValues_mp
import pyvesc
from observer import Subject, Observer
from Display import Display

I2C_ADDR = 0x27
NUM_ROWS = 2
NUM_COLS = 16

# initializing the I2C method for ESP8266
i2c = I2C(scl=Pin(7), sda=Pin(6), freq=10000) 
oled = None
devices = i2c.scan()
if len(devices) == 0:
    Log("No i2c device !")
else:
    Log('i2c devices found:', len(devices))
for device in devices:
    Log("At address: ", hex(device))
    if device == 0x27:
        lcd = LCD(addr=I2C_ADDR, cols=NUM_COLS, rows=NUM_ROWS, i2c=i2c)
        lcd.begin()
        lcd.print("started lcd")
    elif device == 0x3c:
        oled = SSD1306_I2C(128, 64, i2c)
        #oled.text("oled started", 0, 0, 1)
        #oled.show()
        
_wdt = None


_uart = UART(1, 115200, tx=21, rx=20)      # init with given baudrate

_sa = AnalogPins([3, 2])

_np = neopixel.NeoPixel(Pin(10), 69)

'''''''''''
p4 = Pin(4, Pin.IN, Pin.PULL_UP)


def handle_pin_interrupt(pin):
    print("brake change " + str(pin.value()))
    if pin.value():
        # pwm5.duty(10)
        p5.value(0)
    else:
        # pwm5.duty(512)
        p5.value(1)


p4.irq(trigger=3, handler=handle_pin_interrupt)

p5 = Pin(5, mode=Pin.OPEN_DRAIN, pull=None)
'''
# pwm5 = PWM(p5,freq=50000,duty=10)

neopixel_demo.demo(_np)

_vs = VESC(_uart)


if oled:
    oled.text("oled started", 0, 0, 1)
    oled.show()
    d = Display(oled, _np)
    _vs.attach(d)
    _sa.attach(d.ao)
    _wdt = WDT(timeout=4000)  # enable it with a timeout of 2s
    count = 0
    while True:
       
        if count > 100:
            oled.fill(0)
            _sa.update()
            oled.show()
        if _wdt:
            _wdt.feed()
        if _vs.update() > 0:
            _sa.update()
            oled.show()
            count = 0
        else:
            count += 1
        time.sleep(.1)


p = ble_simple_peripheral.ble_main(_uart, _sa, _np, i2c)
if p:
    while True:
        if p.is_connected():
            # Short burst of queued notifications.
            len = _uart.any()
            if len > 0:
                ar = _uart.read(len)
                p.send_uart(ar)
            else: 
                time.sleep_ms(100)
                if _sa:
                    _sa.update()
        else:
            time.sleep(1)





