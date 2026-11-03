.. currentmodule:: machine
.. _machine.WDT:

class WDT -- watchdog timer
===========================

The WDT is used to restart the system when the application crashes and ends
up into a non recoverable state. Once started it cannot be stopped
in any way. After enabling, the application must "feed" the
watchdog periodically to prevent it from expiring and resetting the system.

Example usage::

    from machine import WDT
    wdt = WDT(timeout=2000)  # enable it with a timeout of 2s
    while True:
        # do something useful in less than 2 seconds
        wdt.feed()

Availability: **Alif, ESP32, ESP8266, MIMXRT, RP2, SAMD, STM32, Zephyr**

Constructors
------------

.. class:: WDT(id=0, timeout=5000)

   Create a WDT object and start it. The timeout must be given in milliseconds.
   Once it is running the timeout cannot be changed and the WDT cannot be stopped either.

   Notes:

   - On the alif port the HP and HE cores have independent watchdogs, both accessed
     by the default ``id=0``.  The maximum timeout on the HP core is 10737ms.  The
     watchdog does not run during deepsleep.

   - On the esp8266 port a timeout cannot be specified, it is determined by the underlying
     system.

   - On rp2040 devices the maximum timeout is 8388 ms.

   - On the stm32 port the default ``id=0`` is the IWDG, which can also be specified by
     an id of ``"IWDG"``.  Use an id of ``"WWDG"`` to access the WWDG peripheral.
     For dual-core STM32H7 MCUs there are also ``"IWDG2"`` and ``"WWDG2"``.
     The WWDG has a very limited maximum timeout across all MCUs, of around 100ms (but
     it depends heavily on the APB clock).

Methods
-------

.. method:: WDT.feed()

   Feed the WDT to prevent it from resetting the system. The application
   should place this call in a sensible place ensuring that the WDT is
   only fed after verifying that everything is functioning correctly.
