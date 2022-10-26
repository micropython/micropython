Introduction
============

.. image:: https://readthedocs.org/projects/adafruit-circuitpython-ssd1680/badge/?version=latest
    :target: https://docs.circuitpython.org/projects/ssd1680/en/latest/
    :alt: Documentation Status


.. image:: https://raw.githubusercontent.com/adafruit/Adafruit_CircuitPython_Bundle/main/badges/adafruit_discord.svg
    :target: https://adafru.it/discord
    :alt: Discord


.. image:: https://github.com/adafruit/Adafruit_CircuitPython_SSD1680/workflows/Build%20CI/badge.svg
    :target: https://github.com/adafruit/Adafruit_CircuitPython_SSD1680/actions
    :alt: Build Status


.. image:: https://img.shields.io/badge/code%20style-black-000000.svg
    :target: https://github.com/psf/black
    :alt: Code Style: Black

CircuitPython `displayio` driver for SSD1680-based ePaper displays

Dependencies
=============
This driver depends on:

* `Adafruit CircuitPython <https://github.com/adafruit/circuitpython>`_

Please ensure all dependencies are available on the CircuitPython filesystem.
This is easily achieved by downloading
`the Adafruit library and driver bundle <https://circuitpython.org/libraries>`_
or individual libraries can be installed using
`circup <https://github.com/adafruit/circup>`_.

* Adafruit 2.13" 250x122 Tri-Color eInk / ePaper Display with SRAM - SSD1680 Driver

`Purchase the Breakout from the Adafruit shop <http://www.adafruit.com/products/4947>`_

* Adafruit 2.13" HD Tri-Color eInk / ePaper Display FeatherWing - 250x122 RW Panel with SSD1680

`Purchase the FeatherWing from the Adafruit shop <http://www.adafruit.com/products/4814>`_

Installing from PyPI
=====================

On supported GNU/Linux systems like the Raspberry Pi, you can install the driver locally `from
PyPI <https://pypi.org/project/adafruit-circuitpython-ssd1680/>`_. To install for current user:

.. code-block:: shell

    pip3 install adafruit-circuitpython-ssd1680

To install system-wide (this may be required in some cases):

.. code-block:: shell

    sudo pip3 install adafruit-circuitpython-ssd1680

To install in a virtual environment in your current project:

.. code-block:: shell

    mkdir project-name && cd project-name
    python3 -m venv .venv
    source .venv/bin/activate
    pip3 install adafruit-circuitpython-ssd1680

Usage Example
=============

.. code-block:: python

    import time
    import board
    import displayio
    import adafruit_ssd1680

    displayio.release_displays()

    # This pinout works on a Metro M4 and may need to be altered for other boards.
    spi = board.SPI()  # Uses SCK and MOSI
    epd_cs = board.D9
    epd_dc = board.D10
    epd_reset = board.D8    # Set to None for FeatherWing
    epd_busy = board.D7    # Set to None for FeatherWing

    display_bus = displayio.FourWire(
        spi, command=epd_dc, chip_select=epd_cs, reset=epd_reset, baudrate=1000000
    )
    time.sleep(1)

    display = adafruit_ssd1680.SSD1680(
        display_bus,
        width=250,
        height=122,
        busy_pin=epd_busy,
        highlight_color=0xFF0000,
        rotation=270,
    )

    g = displayio.Group()

    # CircuitPython 6 & 7 compatible
    f = open("/display-ruler.bmp", "rb")
    pic = displayio.OnDiskBitmap(f)
    t = displayio.TileGrid(
        pic, pixel_shader=getattr(pic, "pixel_shader", displayio.ColorConverter())
    )

    # # CircuitPython 7 compatible only
    # pic = displayio.OnDiskBitmap("/display-ruler.bmp")
    # t = displayio.TileGrid(pic, pixel_shader=pic.pixel_shader)

    g.append(t)

    display.show(g)

    display.refresh()
    print("refreshed")

    time.sleep(120)


Documentation
=============

API documentation for this library can be found on `Read the Docs <https://docs.circuitpython.org/projects/ssd1680/en/latest/>`_.

For information on building library documentation, please check out `this guide <https://learn.adafruit.com/creating-and-sharing-a-circuitpython-library/sharing-our-docs-on-readthedocs#sphinx-5-1>`_.

Contributing
============

Contributions are welcome! Please read our `Code of Conduct
<https://github.com/adafruit/Adafruit_CircuitPython_SSD1680/blob/main/CODE_OF_CONDUCT.md>`_
before contributing to help this project stay welcoming.
