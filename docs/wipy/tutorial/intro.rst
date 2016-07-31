Introduction to the WiPy
========================

To get the most out of your WiPy, there are a few basic things to
understand about how it works.

Caring for your WiPy and expansion board
----------------------------------------

Because the WiPy/expansion board does not have a housing it needs a bit of care:

  - Be gentle when plugging/unplugging the USB cable.  Whilst the USB connector
    is well soldered and is relatively strong, if it breaks off it can be very
    difficult to fix.

  - Static electricity can shock the components on the WiPy and destroy them.
    If you experience a lot of static electricity in your area (eg dry and cold
    climates), take extra care not to shock the WiPy.  If your WiPy came
    in a ESD bag, then this bag is the best way to store and carry the
    WiPy as it will protect it against static discharges.

As long as you take care of the hardware, you should be okay.  It's almost
impossible to break the software on the WiPy, so feel free to play around
with writing code as much as you like.  If the filesystem gets corrupt, see
below on how to reset it. In the worst case you might need to do a safe boot,
which is explained in detail :ref:`here <wipy_boot_modes>`.

Plugging into the expansion board and powering on
-------------------------------------------------

The expansion board can power the WiPy via USB. The WiPy comes with a sticker
on top of the RF shield that labels all pins, and this should match the label
numbers on the expansion board headers. When plugging it in, the WiPy antenna
will end up on top of the SD card connector of the expansion board. A video
showing how to do this can be found `here <https://www.youtube.com/watch?v=47D9MZ9zFQw>`_.

Expansion board hardware guide
------------------------------

The document explaining the hardware details of the expansion board can be found
`here <https://github.com/wipy/wipy/blob/master/docs/User_manual_exp_board.pdf>`_.

Powering by an external power source
------------------------------------

The WiPy can be powered by a battery or other external power source.

**Be sure to connect the positive lead of the power supply to VIN, and
ground to GND.  There is no polarity protection on the WiPy so you
must be careful when connecting anything to VIN.**

- When powering via ``VIN``:

   **The input voltage must be between 3.6V and 5.5V.**

- When powering via ``3V3``:

   **The input voltage must be exactly 3V3, ripple free and from a supply capable
   of sourcing at least 300mA of current**

Performing firmware upgrades
----------------------------

For detailed instructions see :ref:`OTA How-To <wipy_firmware_upgrade>`.
