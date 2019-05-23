.. currentmodule:: machine
.. _machine.WDT:

class WDT -- watchdog timer
===========================

The WDT is used to restart the system when the application crashes and ends
up into a non recoverable state. Once started it cannot be stopped or
reconfigured in any way. After enabling, the application must "feed" the
watchdog periodically to prevent it from expiring and resetting the system.

Example usage::

    from machine import WDT
    wdt = WDT(0,5000000)
    wdt.feed()

Availability of this class: W60x.

Constructors
------------

.. class:: WDT(id=0, timeout=5000000)

   Create a WDT object and start it. The timeout must be given in microsecond and
   the minimum value that is accepted is 2 milliseconds. Once it is running the timeout
   cannot be changed and the WDT cannot be stopped either.
   Default time 5000000 microseconds.

Methods
-------

.. method:: wdt.feed()

   Feed the WDT to prevent it from resetting the system. The application
   should place this call in a sensible place ensuring that the WDT is
   only fed after verifying that everything is functioning correctly.
