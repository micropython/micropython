.. _pyb.WDT:

class WDT -- watchdog timer
===========================

The WDT is used to restart the system when the application crashes and ends
up into a non recoverable state. Once started it cannot be stopped or
reconfigured in any way. After enabling, the application must "kick" the
watchdog periodically to prevent it from expiring and resetting the system.

Example usage::

    wdt = pyb.WDT(5000) # enable with a timeout of 5s
    wdt.kick()

Constructors
------------

.. class:: pyb.WDT([timeout])

   Create a WDT object. If the timeout is specified the WDT is started.
   The timeout must be given in seconds and 1s the minimum value that
   is accepted. Once it is running the timeout cannot be changed and
   the WDT cannot be stopped either.

Methods
-------

.. method:: wdt.kick()

   Kick the WDT to prevent it from resetting the system. The application
   should place this call in a sensible place ensuring that the WDT is
   only kicked after verifying that everything is functioning correctly.
