.. currentmodule:: network
.. _network.NinaW10:

class NinaW10 -- control WiFi modules with NINA firmware
========================================================

This class allows you to control WLAN modules based on the NINA firmware,
like the u.Blox W102, Adafruit Airlift modules or genuine ESP32 boards..

Example usage::

    import network
    from machine import SPI, Pin
    spi=SPI(1, 8000000, sck="SCK", mosi="MOSI", miso="MISO")
    wlan = network.NinaW10(network.STA_IF, spi=spi, cs=Pin("D13"), busy=Pin("D11"), reset=Pin("D12"))

    # now use socket as usual
    ...

For this example to work the NINA module must have the following connections:

    - the SPI interface with MOSI, MISO and SCK
    - cs connected to pin D13
    - busy connected to D11
    - reset connected to D12

It is possible to use other SPI buses and other pins for cs, busy and reset.

Constructors
------------

.. class:: NinaW10(interface, *, spi=spi, cs=pin_cs, busy=pin_busy, reset=pin_rst)

   Create a NinaW10 driver object, initialise the NINA module using the given
   SPI bus and pins, and return the Wlan object.

   Arguments are:

     - *interface* is the interface mode, either network.STA_IF or network.AP_IF. If
       it is omitted, network.STA_IF is assumed.
     - *spi* is an :ref:`SPI object <machine.SPI>` which is the SPI bus that the NINA module is
       connected to (the MOSI, MISO and SCLK pins).
     - *pin_cs* is a :ref:`Pin object <machine.Pin>` which is connected to the NINA CS pin.
     - *pin_busy* is a :ref:`Pin object <machine.Pin>` which is connected to the NINA busy pin.
     - *pin_rst* is a :ref:`Pin object <machine.Pin>` which is connected to the NINA reset pin.

   The SPI object must be initialized beforehand. The mode of the Pin objects will be
   set by the driver.

The default pins for some boards and extensions are:

========  =========  =======  =======  =========   =========
Airlift   ItsyBitsy  Feather  Airlift  METRO M4    Sparkfun
Breakout  Add-on     Add-on   shield   Airlift     SAMD51_TP
========  =========  =======  =======  =========   =========
MOSI      MOSI       MOSI     D11      MOSI        MOSI
MISO      MISO       MISO     D12      MISO        MISO
SCK       SCK        SCK      D13      SCK         SCK
CS        D13        D13      D10      ESP_CS      D13
Busy      D11        D11      D7       ESP_BUSY    D11
Reset     D12        D12      D5       ESP_RESET   D12
GP0       D10        D10      D6       ESP_GP0     D10
========  =========  =======  =======  =========   =========


A useful function for connecting to your local WiFi network is::

    def do_connect():
        from machine import SPI, Pin
        import time

        spi=SPI(1, 8000000, sck="SCK", mosi="MOSI", miso="MISO")
        wlan = network.WLAN(network.STA_IF, spi=spi, cs=Pin("D13"), busy=Pin("D11"), reset=Pin("D12"))
        wlan.active(True)
        if not wlan.isconnected():
            wlan.connect(WIFI_SSID, WIFI_PASSWD) # connect to an AP

            for _ in range(20):
                if wlan.isconnected():
                    print('\nnetwork config:', wlan.ifconfig())
                    break
                print(".", end="")
                time.sleep(0.5)
            else:
                print(" Connect attempt timed out\n")

Once the network is established the :mod:`socket <socket>` module can be used
to create and use TCP/UDP sockets as usual. WLAN support is at the moment
only supported for SAM51 boards with a matching add-on board.

Note:

  - If feasible, pull the RESET line low with an external resistor. The
    required value depends on the NINA module. A good value is 2.2 kOhm.
    This forces the GPIO lines of the module to high impedance when
    not being used.

Methods
-------

The methods of the WLAN class are supported. For further details,
see :ref:`WLAN class <network.WLAN>`
