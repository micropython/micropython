.. currentmodule:: machine
.. _machine.WDT:

class WDT -- watchdog timer
===========================

The WDT is used to restart the system when the application crashes and ends
up into a non recoverable state. Once started it cannot be stopped or
reconfigured in any way (Not always, see wdt.deinit()). After enabling, the application must "feed" the 
watchdog periodically to prevent it from expiring and resetting the system.

Example usage::

    from machine import WDT
    wdt = WDT(timeout=2000)  # enable it with a timeout of 2s
    while True:
        # do something

        if verify_that_everything_is_functioning_correctly():
            wdt.feed()

Availability of this class: pyboard, WiPy, esp8266, esp32.

Constructors
------------
 
.. class:: WDT(id=0, timeout=5000)

   Create a WDT object and start it. The timeout must be given in milliseconds.
   Once it is running the timeout cannot be changed and the WDT cannot be stopped either.
   (Not always, see wdt.deinit())
   
   Notes: On the esp32 the minimum timeout is 1 second. On the esp8266 a timeout
   cannot be specified, it is determined by the underlying system.

Methods
-------

.. method:: wdt.feed()

   Feed the WDT to prevent it from resetting the system. The application
   should place this call in a sensible place ensuring that the WDT is
   only fed after verifying that everything is functioning correctly.

.. method:: wdt.deinit()

   This is exception from the rule.
   Use this method to stop the watchdog timer during normal code execution
   before exit to REPL(WebREPL). Than you can manually restart this code, 
   start another code or reboot MCU. This may be need in debug mode.  
   Don't use it in production/release code.

   Example usage::

      import sys
      from machine import WDT

      wdt = WDT(timeout=1000)

      try:
         while True:
            # do something
   
            if verify_that_everything_is_functioning_correctly():
               wdt.feed()

            if need_to_exit_without_reboot():
               wdt.deinit()
               sys.exit()
               # will enter to REPL(WebREPL) 
      except:
         raise
         # WDT will reboot the MCU after any exceptions

   Availability of this method: esp32.  
