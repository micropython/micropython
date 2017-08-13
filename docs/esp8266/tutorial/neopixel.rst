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

The following demo function makes a fancy show on the LEDs::

    import time

    def demo(np):
        n = np.n

        # cycle
        for i in range(4 * n):
            for j in range(n):
                np[j] = (0, 0, 0)
            np[i % n] = (255, 255, 255)
            np.write()
            time.sleep_ms(25)

        # bounce
        for i in range(4 * n):
            for j in range(n):
                np[j] = (0, 0, 128)
            if (i // n) % 2 == 0:
                np[i % n] = (0, 0, 0)
            else:
                np[n - 1 - (i % n)] = (0, 0, 0)
            np.write()
            time.sleep_ms(60)

        # fade in/out
        for i in range(0, 4 * 256, 8):
            for j in range(n):
                if (i // 256) % 2 == 0:
                    val = i & 0xff
                else:
                    val = 255 - (i & 0xff)
                np[j] = (val, 0, 0)
            np.write()

        # clear
        for i in range(n):
            np[i] = (0, 0, 0)
        np.write()

Execute it using::

    >>> demo(np)
