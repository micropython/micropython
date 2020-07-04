# Driver test for official MicroPython LCD160CR display
# MIT license; Copyright (c) 2017 Damien P. George

import time, math, framebuf, lcd160cr


def get_lcd(lcd):
    if type(lcd) is str:
        lcd = lcd160cr.LCD160CR(lcd)
    return lcd


def show_adc(lcd, adc):
    data = [adc.read_core_temp(), adc.read_core_vbat(), 3.3]
    try:
        data[2] = adc.read_vref()
    except:
        pass
    for i in range(3):
        lcd.set_text_color((825, 1625, 1600)[i], 0)
        if lcd.h == 160:
            lcd.set_font(2)
            lcd.set_pos(0, 100 + i * 16)
        else:
            lcd.set_font(2, trans=1)
            lcd.set_pos(0, lcd.h - 60 + i * 16)
        lcd.write("%4s: " % ("TEMP", "VBAT", "VREF")[i])
        if i > 0:
            s = "%6.3fV" % data[i]
        else:
            s = "%5.1f°C" % data[i]
        if lcd.h == 160:
            lcd.set_font(1, bold=0, scale=1)
        else:
            lcd.set_font(1, bold=0, scale=1, trans=1)
            lcd.set_pos(45, lcd.h - 60 + i * 16)
        lcd.write(s)


def test_features(lcd, orient=lcd160cr.PORTRAIT):
    # if we run on pyboard then use ADC and RTC features
    try:
        import pyb

        adc = pyb.ADCAll(12, 0xF0000)
        rtc = pyb.RTC()
    except:
        adc = None
        rtc = None

    # set orientation and clear screen
    lcd = get_lcd(lcd)
    lcd.set_orient(orient)
    lcd.set_pen(0, 0)
    lcd.erase()

    # create M-logo
    mlogo = framebuf.FrameBuffer(bytearray(17 * 17 * 2), 17, 17, framebuf.RGB565)
    mlogo.fill(0)
    mlogo.fill_rect(1, 1, 15, 15, 0xFFFFFF)
    mlogo.vline(4, 4, 12, 0)
    mlogo.vline(8, 1, 12, 0)
    mlogo.vline(12, 4, 12, 0)
    mlogo.vline(14, 13, 2, 0)

    # create inline framebuf
    offx = 14
    offy = 19
    w = 100
    h = 75
    fbuf = framebuf.FrameBuffer(bytearray(w * h * 2), w, h, framebuf.RGB565)
    lcd.set_spi_win(offx, offy, w, h)

    # initialise loop parameters
    tx = ty = 0
    t0 = time.ticks_us()

    for i in range(300):
        # update position of cross-hair
        t, tx2, ty2 = lcd.get_touch()
        if t:
            tx2 -= offx
            ty2 -= offy
            if tx2 >= 0 and ty2 >= 0 and tx2 < w and ty2 < h:
                tx, ty = tx2, ty2
        else:
            tx = (tx + 1) % w
            ty = (ty + 1) % h

        # create and show the inline framebuf
        fbuf.fill(lcd.rgb(128 + int(64 * math.cos(0.1 * i)), 128, 192))
        fbuf.line(
            w // 2,
            h // 2,
            w // 2 + int(40 * math.cos(0.2 * i)),
            h // 2 + int(40 * math.sin(0.2 * i)),
            lcd.rgb(128, 255, 64),
        )
        fbuf.hline(0, ty, w, lcd.rgb(64, 64, 64))
        fbuf.vline(tx, 0, h, lcd.rgb(64, 64, 64))
        fbuf.rect(tx - 3, ty - 3, 7, 7, lcd.rgb(64, 64, 64))
        for phase in (-0.2, 0, 0.2):
            x = w // 2 - 8 + int(50 * math.cos(0.05 * i + phase))
            y = h // 2 - 8 + int(32 * math.sin(0.05 * i + phase))
            fbuf.blit(mlogo, x, y)
        for j in range(-3, 3):
            fbuf.text(
                "MicroPython",
                5,
                h // 2 + 9 * j + int(20 * math.sin(0.1 * (i + j))),
                lcd.rgb(128 + 10 * j, 0, 128 - 10 * j),
            )
        lcd.show_framebuf(fbuf)

        # show results from the ADC
        if adc:
            show_adc(lcd, adc)

        # show the time
        if rtc:
            lcd.set_pos(2, 0)
            lcd.set_font(1)
            t = rtc.datetime()
            lcd.write(
                "%4d-%02d-%02d %2d:%02d:%02d.%01d"
                % (t[0], t[1], t[2], t[4], t[5], t[6], t[7] // 100000)
            )

        # compute the frame rate
        t1 = time.ticks_us()
        dt = time.ticks_diff(t1, t0)
        t0 = t1

        # show the frame rate
        lcd.set_pos(2, 9)
        lcd.write("%.2f fps" % (1000000 / dt))


def test_mandel(lcd, orient=lcd160cr.PORTRAIT):
    # set orientation and clear screen
    lcd = get_lcd(lcd)
    lcd.set_orient(orient)
    lcd.set_pen(0, 0xFFFF)
    lcd.erase()

    # function to compute Mandelbrot pixels
    def in_set(c):
        z = 0
        for i in range(32):
            z = z * z + c
            if abs(z) > 100:
                return i
        return 0

    # cache width and height of LCD
    w = lcd.w
    h = lcd.h

    # create the buffer for each line and set SPI parameters
    line = bytearray(w * 2)
    lcd.set_spi_win(0, 0, w, h)
    spi = lcd.fast_spi()

    # draw the Mandelbrot set line-by-line
    hh = (h - 1) / 3.2
    ww = (w - 1) / 2.4
    for v in range(h):
        for u in range(w):
            c = in_set((v / hh - 2.3) + (u / ww - 1.2) * 1j)
            if c < 16:
                rgb = c << 12 | c << 6
            else:
                rgb = 0xF800 | c << 6
            line[2 * u] = rgb
            line[2 * u + 1] = rgb >> 8
        spi.write(line)


def test_all(lcd, orient=lcd160cr.PORTRAIT):
    lcd = get_lcd(lcd)
    test_features(lcd, orient)
    test_mandel(lcd, orient)


print("To run all tests: test_all(<lcd>)")
print("Individual tests are: test_features, test_mandel")
print('<lcd> argument should be a connection, eg "X", or an LCD160CR object')
