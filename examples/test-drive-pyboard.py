# Code samples from Test drive a pyboard online:
# http://micropython.org/live/

# ----------------------------------------

# turn on an LED and print text
import pyb
pyb.LED(1).on()
print('Hello Micro Python!')

# ----------------------------------------

# flash LEDs randomly
import pyb
leds = [pyb.LED(i+1) for i in range(4)]
for i in range(100):
    leds[pyb.rng() % len(leds)].toggle()
    pyb.delay(50)

# ----------------------------------------

# pulse the intensite of the blue LED
import pyb, math
for i in range(200):
    pyb.LED(4).intensity(int(255 * math.sin(i * math.pi / 100)**2))
    pyb.delay(25)

# ----------------------------------------

# bignum
print(1 << 1000)

# ----------------------------------------

# inline assembler
@micropython.asm_thumb
def asm_add(r0, r1):
    add(r0, r0, r1)
print(asm_add(1, 2))

# ----------------------------------------

# filesystem
import os

# list root directory
print(os.listdir('/'))

# print current directory
print(os.getcwd())

# open and read a file from the SD card
with open('/sd/readme.txt') as f:
    print(f.read())

# ----------------------------------------

# GPIO
from pyb import Pin

# create output and input pins (connected together)
pin_out = Pin('Y9', Pin.OUT_PP)
pin_in = Pin('Y10', Pin.IN)

# set output and read input pins
pin_out.high()
print(pin_in.value())
pin_out.low()
print(pin_in.value())

# ----------------------------------------

# UART
from pyb import UART

# create UART object (on Y9, Y10, connected together)
uart = UART(3, baudrate=9600)

# write data
uart.write('hello!')

# read data
print(uart.read())

# ----------------------------------------

# timer interrupts
from pyb import LED, Timer

# create a timer running at 2Hz
tim = Timer(1, freq=2)

# set a callback to be called at 2Hz
tim.callback(lambda t: LED(2).toggle())

# ----------------------------------------

# PWM and ADC
import pyb
from pyb import Pin, Timer, ADC

# create 50% duty cycle at 10Hz on pin X6
ch = Timer(2, freq=10).channel(1, Timer.PWM, pin=Pin('X6'))
ch.pulse_width_percent(50)

# read PWM output using ADC on pin X19
pin = ADC(Pin('X19'))
for x in range(50):
    print('-' * int(40 * pin.read() / 4095), 'O', sep='')
    pyb.delay(10)

# ----------------------------------------

# DAC and ADC
import pyb, math
from pyb import Pin, DAC, ADC

# create a buffer containing a sine-wave
buf = bytearray(100)
for i in range(len(buf)):
    buf[i] = 128 + int(127 * math.sin(2 * math.pi * i / len(buf)))

# output the sine-wave at 10Hz on pin X6
dac = DAC(2)
dac.write_timed(buf, 10 * len(buf), mode=DAC.CIRCULAR)

# read DAC output using ADC on pin X19
pin = ADC(Pin('X19'))
for x in range(50):
    print('-' * int(40 * pin.read() / 4095), 'O', sep='')
    pyb.delay(5)

# ----------------------------------------

# servo control
from pyb import Servo

s1 = Servo(1) # an angle servo
s2 = Servo(2) # a continuous rotation servo

# move to 45 degrees; wait 1 second
s1.angle(45)
pyb.delay(1000)

# move to -60 degrees over 1.5 seconds
s1.angle(-60, 1500)

# speed up to a speed of 20 over 2 seconds
s2.speed(20, 1000)

# wait 3 seconds then stop servo
pyb.delay(3000)
s2.speed(0)

# ----------------------------------------

# CPU frequency control
import pyb

sysclk = 8, 16, 32, 64, 96, 168

for s in sysclk:
    # change CPU frequency, given in MHz
    pyb.freq(s * 1000000)
    # re-init UART baudrate so print works
    pyb.repl_uart().init(baudrate=115200)
    # print new frequency
    print(pyb.freq())

# ----------------------------------------

# LCD output
import pyb
lcd = pyb.LCD('Y')
lcd.light(True)
lcd.write('Hello,\n\n  Micro Python!\n')

# ----------------------------------------

# Conway's Game of Life
import pyb

PIXEL_SIZE = const(4)

# we use big pixels so it's easier to see
@micropython.viper
def big_pixel(x:int, y:int, col):
    pixel = lcd.pixel
    for i in range(PIXEL_SIZE):
        for j in range(PIXEL_SIZE):
            pixel(x + i, y + j, col)

# do 1 iteration of Conway's Game of Life
@micropython.viper
def conway_step(lcd):
    get = lcd.get
    for x in range(0, 128, PIXEL_SIZE):
        for y in range(0, 32, PIXEL_SIZE):
            self = int(get(x, y))

            # count number of neigbours
            neighbours = int(
                get(x - PIXEL_SIZE, y - PIXEL_SIZE) +
                get(x, y - PIXEL_SIZE) +
                get(x + PIXEL_SIZE, y - PIXEL_SIZE) +
                get(x - PIXEL_SIZE, y) +
                get(x + PIXEL_SIZE, y) +
                get(x + PIXEL_SIZE, y + PIXEL_SIZE) +
                get(x, y + PIXEL_SIZE) +
                get(x - PIXEL_SIZE, y + PIXEL_SIZE)
            )

            # apply the rules of life
            if self and not 2 <= neighbours <= 3:
                big_pixel(x, y, 0)
            elif not self and neighbours == 3:
                big_pixel(x, y, 1)

# create LCD object
lcd = pyb.LCD('Y')
lcd.light(True)

# randomise the start
for x in range(0, 128, PIXEL_SIZE):
    for y in range(0, 32, PIXEL_SIZE):
        big_pixel(x, y, pyb.rng() & 1)
lcd.show()

# run Conway's Game of Life
for i in range(50):
    conway_step(lcd)
    lcd.show()
    pyb.delay(100)

# ----------------------------------------

# driver for Adafruit neo pixels (WS2812 chip)
import pyb

class NeoPixel:
    def __init__(self, spi, n):
        spi.init(spi.MASTER, baudrate=2625000)
        spi.send(0)
        self.spi = spi
        self.buf = bytearray(9 * n + 1)
        self.clear()

    def __len__(self):
        return (len(self.buf) - 1) // 9

    def __setitem__(self, idx, val):
        idx *= 9
        self.fillword(idx, val[1]) # green
        self.fillword(idx + 3, val[0]) # red
        self.fillword(idx + 6, val[2]) # blue

    def clear(self):
        for i in range(len(self)):
            self[i] = (0, 0, 0)

    def send(self):
        self.spi.send(self.buf)

    def fillword(self, idx, val):
        expval = 0
        for i in range(8):
            expval <<= 3
            if val & 0x80:
                expval |= 0b110
            else:
                expval |= 0b100
            val <<= 1
        self.buf[idx] = expval >> 16
        self.buf[idx + 1] = expval >> 8
        self.buf[idx + 2] = expval

# create a driver on SPI(1) with 8 pixels
ws = NeoPixel(pyb.SPI(1), 8)

# scroll one white pixel
for i in range(48):
    ws.clear()
    ws[i % len(ws)] = (64, 64, 64)
    ws.send()
    pyb.delay(50)

# turn all pixels off
ws.clear()
ws.send()

# ----------------------------------------
