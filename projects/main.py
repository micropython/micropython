

from machine import UART,WDT,Pin,I2C
from observer_analog import AnalogPins
import ble_simple_peripheral
from log import Log

I2C_ADDR = 0x27
totalRows = 2
totalColumns = 16

#i2c = SoftI2C(scl=Pin(22), sda=Pin(21), freq=10000)     #initializing the I2C method for ESP32
i2c = I2C(scl=Pin(5), sda=Pin(4), freq=10000)       #initializing the I2C method for ESP8266

devices = i2c.scan()
if len(devices) == 0:
    Log("No i2c device !")
else:
    Log('i2c devices found:',len(devices))
for device in devices:
    Log("At address: ",hex(device))

#lcd = I2cLcd(i2c, I2C_ADDR, totalRows, totalColumns)

#_wdt = WDT(timeout=2000)  # enable it with a timeout of 2s

_uart = UART(1, 115200,tx=21, rx=20)                         # init with given baudrate

_sa =AnalogPins([3,2])



#ble_simple_peripheral.ble_main(_uart,_sa,_wdt)

ble_simple_peripheral.ble_main(_uart,_sa)






