

from machine import UART, WDT, Pin, I2C
import neopixel
import time
from observer_analog import AnalogPins
from observer_vesc import VESC
import ble_simple_peripheral
import neopixel_demo
from log import Log
import micropython_dotstar as dotstar
from lcd_i2c import LCD
from ssd1306 import SSD1306_I2C
from Display import DisplayDispatcher,  DisplayOLED, DisplayNeopixel, Display4LineLCD
import us2n

I2C_ADDR = 0x27
NUM_ROWS = 4
NUM_COLS = 20

# initializing the I2C method for ESP8266
i2c = I2C(scl=Pin(7), sda=Pin(6), freq=10000) 
oled = None
lcd = None
_np = None
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
        lcd.print("lcd availible")
    elif device == 0x3c:
        oled = SSD1306_I2C(128, 64, i2c)
        # oled.text("oled started", 0, 0, 1)
        # oled.show()
        
_wdt = None

_uart = UART(1, 115200, tx=21, rx=20)      # init with given baudrate

_sa = AnalogPins([3, 2])

server = us2n.server('us2n.json', _uart, lcd)


# _np = neopixel.NeoPixel(Pin(10), 69)

if _np:
    neopixel_demo.demo(_np)

_vs = VESC(_uart)


if oled:
    oled.text("oled started", 0, 0, 1)
    oled.show()
    
if lcd:
    lcd.set_cursor(0, 1)
    lcd.print("Started LCD")

if lcd or oled: 
    if oled:
        ds = DisplayOLED(oled)
    if lcd:
        ds = Display4LineLCD(lcd)
    # d = Display(oled, lcd, _np)
    d = DisplayDispatcher(ds)

    if _np:
        ds = DisplayNeopixel(_np)
        d.append(ds)

    _vs.attach(d.vo)
    _sa.attach(d.ao)
    _wdt = WDT(timeout=4000)  # enable it with a timeout of 2s
    count = 0
    while True:
       
        if count > 10:
            _sa.update()
            ds.show()
            count = 0
        if _wdt:
            _wdt.feed()
        if _vs.update() > 0:
            _sa.update()
            ds.show()
            count = 0
        else:
            count += 1
        time.sleep(.1)


server.serve_forever()


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





