name = "m5core2"

def init():
    # Init LVGL
    import lvgl as lv
    lv.init()

    # Power Management
    from power import Power
    Power()

    # LCD screen
    from ili9XXX import ili9341
    ili9341(mosi=23, miso=38, clk=18, dc=15, cs=5, invert=True, rot=0x10, width=320, height=240, rst=-1, power=-1, backlight=-1)

    # Touch sensor
    from ft6x36 import ft6x36
    ft6x36(width=320, height=280)
