:mod:`neopixel` --- control of WS2812 / NeoPixel LEDs
=====================================================

.. module:: neopixel
   :synopsis: control of WS2812 / NeoPixel LEDs

This module provides a driver for WS2818 / NeoPixel LEDs.

.. note:: This module is only included by default on the ESP8266, ESP32 and RP2
   ports. On STM32 / Pyboard and others, you can either install the
   ``neopixel`` package using :term:`mip`, or you can download the module
   directly from :term:`micropython-lib` and copy it to the filesystem.

class NeoPixel
--------------

This class stores pixel data for a WS2812 LED strip connected to a pin. The
application should set pixel data and then call :meth:`NeoPixel.write`
when it is ready to update the strip.

For example::

    import neopixel

    # 32 LED strip connected to X8.
    p = machine.Pin.board.X8
    np = neopixel.NeoPixel(p, 32)

    # Draw a red gradient.
    for i in range(32):
        np[i] = (i * 8, 0, 0)

    # Update the strip.
    np.write()

Constructors
------------

.. class:: NeoPixel(pin, n, *, bpp=3, timing=1)

    Construct an NeoPixel object.  The parameters are:

        - *pin* is a machine.Pin instance.
        - *n* is the number of LEDs in the strip.
        - *bpp* is 3 for RGB LEDs, and 4 for RGBW LEDs.
        - *timing* is 0 for 400KHz, and 1 for 800kHz LEDs (most are 800kHz). You
          may also supply a timing tuple as accepted by `machine.bitstream()`.

Pixel access methods
--------------------

.. method:: NeoPixel.fill(pixel)

    Sets the value of all pixels to the specified *pixel* value (i.e. an
    RGB/RGBW tuple).

.. method:: len(np)

    Returns the number of LEDs in the strip for NeoPixel *np*.

.. method:: np[index]

    Returns the pixel of NeoPixel *np* at *index* as an RGB/RGBW tuple.

.. method:: np[index] =  value

    Set the pixel of NeoPixel *np* at *index* to the value, which is an RGB/RGBW tuple.

Output methods
--------------

.. method:: NeoPixel.write()

    Writes the current pixel data to the strip.
