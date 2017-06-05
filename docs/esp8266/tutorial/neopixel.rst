Controlling NeoPixels
=====================

NeoPixels, also known as WS2812 LEDs, are full-colour LEDs that are connected in
serial, are individually addressable, and can have their red, green and blue
components set between 0 and 255.  They require precise timing to control them
and there is a special neopixel module to do just this.

To create a NeoPixel object do the following::

    >>> import machine, neopixel
    >>> np = neopixel.NeoPixel(machine.Pin(4), 8)

This configures a NeoPixel strip on GPIO4 with 8 pixels.  You can adjust the
"4" (pin number) and the "8" (number of pixel) to suit your set up.

To set the colour of pixels use::

    >>> np[0] = (255, 0, 0) # set to red, full brightness
    >>> np[1] = (0, 128, 0) # set to green, half brightness
    >>> np[2] = (0, 0, 64)  # set to blue, quarter brightness

Then use the ``write()`` method to output the colours to the LEDs::

    >>> np.write()

The following demo functions make a fancy show on the LEDs::

    import time
    from urandom import getrandbits
    from machine import Pin
    from neopixel import NeoPixel

    def cycle(np, color, sleep):
        n = np.n
        for i in range(4 * n):
            for j in range(n):
                np[j] = (0, 0, 0)
            np[i % n] = color
            np.write()
            time.sleep_ms(sleep)

    def bounce(np, color, sleep):
        n = np.n
        for i in range(4 * n):
            for j in range(n):
                np[j] = (0, 0, 128)
            if (i // n) % 2 == 0:
                np[i % n] = (0, 0, 0)
            else:
                np[n - 1 - (i % n)] = (0, 0, 0)
            np.write()
            time.sleep_ms(sleep)

    def fade(np):
        n = np.n
        for i in range(0, 4 * 256, 8):
            for j in range(n):
                if (i // 256) % 2 == 0:
                    val = i & 0xff
                else:
                    val = 255 - (i & 0xff)
                np[j] = (val, 0, 0)
            np.write()

    def colorWipe(np, color, sleep):
        for i in range(np.n):
            np[i] = color
            np.write()
            time.sleep_ms(sleep)

    def theatreChase(np, color, sleep):
        for n in range(10):
            # 3 unique combinations in the pattern [100,010,001]
            for u in range(3):
                for i in range(np.n):
                    # turn every third pixel on, the others off
                    if i % 3 == u:
                        np[i] = color
                    else:
                        np[i] = (0, 0, 0)
                np.write()
                time.sleep_ms(sleep)

    def theatreChaseRainbow(np, sleep):
        # cycle through all 256 colours, skip 2/3s
        for n in range(86):
            # 3 unique combinations in the pattern [100,010,001]
            for u in range(3):
                for i in range(np.n):
                    # turn every third pixel on, the others off
                    if i % 3 == u:
                        np[i] = wheel((i + n * 3) % 255)
                    else:
                        np[i] = (0, 0, 0)
                np.write()
                time.sleep_ms(sleep)

    def rainbow(np):
        for i in range(np.n):
            np[i] = wheel((i * 256 // np.n) % 255)

    def rainbowFade(np, sleep):
        for n in range(256):
            for i in range(np.n):
                np[i] = wheel((i + n) & 255)
            np.write()
            time.sleep_ms(sleep)

    def rainbowCycle(np, sleep):
        # 5 cycles of all colors on wheel
        for r in range(5):
            for n in range(256):
                for i in range(np.n):
                    np[i] = wheel(((i * 256 // np.n) + n) & 255)
                np.write()
            time.sleep_ms(sleep)

    def randomColors(np, sleep):
        for r in range(50):
            for i in range(np.n):
                np[i] = (getrandbits(8), getrandbits(8), getrandbits(8))
            np.write()
            time.sleep_ms(sleep)

    def clear(np):
        n = np.n
        for i in range(n):
            np[i] = (0, 0, 0)
        np.write()

    # Helper for converting 0-255 offset to a colour tuple
    def wheel(offset):
        # The colours are a transition r - g - b - back to r
        offset = 255 - offset
        if offset < 85:
            return (255 - offset * 3, 0, offset * 3)
        if offset < 170:
            offset -= 85
            return (0, offset * 3, 255 - offset * 3)
        offset -= 170
        return (offset * 3, 255 - offset * 3, 0)

Execute them using::

    np = NeoPixel(Pin(4, Pin.OUT), 8)

    # Predefine some colours
    red = (255,0,0)
    green = (0,255,0)
    blue = (0,0,255)
    cyan = (0,255,255)
    magenta = (255,0,255)
    yellow = (255,255,0)
    white = (255,255,255)
    black = (0,0,0)
    colors = [red,green,blue,cyan,magenta,yellow,white,black]

    # Illuminate the pixels one by one
    for color in colors:
        cycle(np, color, 50)

    # Bounce a dark pixel back and forth
    bounce(np, red, 60)

    # Fade in/out
    fade(np)

    # Illuminate the pixels one by one, keeping them lit
    for color in colors:
        colorWipe(np, color, 50)

    # Theatre-style crawling lights, in a given colour
    theatreChase(np, red, 100)

    # Theatre-style crawling lights, in rainbow colours
    theatreChaseRainbow(np, 100)

    # Show all colours of the rainbow
    rainbow(np)

    # Fade all pixels together through rainbow colours
    rainbowFade(np, 25)

    # Fade all pixels together through rainbow colours, offset each pixel
    rainbowCycle(np, 25)

    # Random colours
    randomColors(np, 50)

    # Turn off all pixels
    clear(np)
