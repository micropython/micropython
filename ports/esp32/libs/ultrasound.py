'''Controller for using the maxbotics sensor'''

from machine import ADC, Pin, UART
from time import sleep_ms, time
# _sensor = ADC(Pin(35))
# _sensor.atten(ADC.ATTN_11DB)
# 12
_load_switch = Pin(27, Pin.OUT)
uart = UART(1,9600)
uart.init(baudrate=9600, bits=8, parity=None, stop=1, tx=32, rx=18, invert=UART.INV_RX)
sensor = ADC(Pin(35))
sensor.atten(ADC.ATTN_11DB)
# uart = UART.init(baudrate=9600, bits=8, parity=None, stop=1, tx=34, rx=35)


def get_reading_mm() -> int:
    '''Return the sensor reading converted to mm'''
    for _ in range(10):
        try:
            data = uart.read()
            data = data.decode('ascii').split('R')
            if len(data) > 1:
                measurement = data[1][:-1]
                print(measurement)
                measurement = int(measurement)*10
                return measurement
        except:
            ...
        sleep_ms(200)

    raise Exception

def get_analogue(period = 0.5):
    start_time = time()
    readings = []
    while time() - start_time < 0.5:
        reading = sensor.read()
        readings.append(reading)
    return readings



def get_reading_raw() -> int:
    '''Return the raw value from the sensor'''
    return get_reading_mm()

def turn_off_sensor():
    print('turning off sensor')
    # turn the load switch off
    _load_switch.value(0)

def turn_on_sensor():
    print('turning on sensor')
    _load_switch.value(1)
