# Driver test for SSD1306 display with SPI connection
# MIT license; Copyright (c) 2017 Zoltan Gyarmati
import machine

width = 128
height = 64

def draw_testscreen(display):
    try:
        import pyb
        rtc = pyb.RTC()
    except:
        rtc = None

    # create M-logo
    display.fill(0)
    display.fill_rect(1, 1, 15, 15,1)
    display.vline(4, 4, 12, 0)
    display.vline(8, 1, 12, 0)
    display.vline(12, 4, 12, 0)
    display.vline(14, 13, 2, 0)
    display.text('MicroPython',20,0)

    display.hline(0,18,128,1)
    # show the time
    if rtc:
        dt = rtc.datetime()
        d = '%4d-%02d-%02d' % (dt[0], dt[1], dt[2])
        t = '%2d:%02d:%02d.%01d' % (dt[4], dt[5], dt[6], dt[7] // 100000)
        display.text(d,20,32)
        display.text(t,20,40)
    display.show()


def test_spi():
    """
    Wiring for STM32F4DISCOVERY, SPI bus #1
    DISPLAY -->  DISCOVERY
    DC      -->  PB1  (data/command)
    RES     -->  PA3  (reset)
    CS      -->  PC5  (Chip Select)
    D0      -->  PA5  SPI SCK
    D1      -->  PA7  SPI MOSI
    GND     -->  GND
    VCC     -->  5V
    """
    from ssd1306 import SSD1306_SPI
    bus = machine.SPI(1)
    dc = machine.Pin('PB1')
    res = machine.Pin('PA3')
    cs =  machine.Pin('PC5')
    d = SSD1306_SPI(width, height, bus, dc, res, cs)
    draw_testscreen(d)


def test_i2c():
    """
    Wiring for STM32F4DISCOVERY, I2C bus #1
    DISPLAY -->  DISCOVERY
    DC      -->  GND (addr: 0x3C, if set to VCC, addr is 0x3D)
    RES     -->  PB4 (reset)
    CS      -->  GND (insignificant on most displays)
    D0      -->  PB6 (S2C SCL)
    D1      -->  PB7 (I2C SDA)
    GND     -->  GND
    VCC     -->  5V
    """
    from ssd1306 import SSD1306_I2C
    res = machine.Pin('PB4')
    bus = machine.I2C(1)
    d = SSD1306_I2C(width,height,bus,0x3C,res)
    draw_testscreen(d)
