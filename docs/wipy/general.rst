General information about the WiPy
==================================

WLAN default behaviour
----------------------

When the WiPy boots with the default factory configuration starts in Access Point 
mode with ``ssid: wipy-wlan`` and ``key: www.wipy.io``. 
Connect to this network and the WiPy will be reachable at ``192.168.1.1``. In order
to gain access to the interactive prompt, open a telnet session to that IP address on
the default port (23). You will be asked for credentials:
``login: micro``  ``password: python``

Local file system and SD card
-----------------------------

There is a small internal file system (a drive) on the WiPy, called ``/flash``,
which is stored within the external serial flash memory.  If a micro SD card
is hooked-up and enabled, it is available as ``/sd``.

When the WiPy boots up, it always boots from the ``boot.py`` located in the 
``/flash`` file system.  If during the boot process the SD card is enabled and
it's selected as the current drive then the WiPy will try to execute ``main.py``
that should be located in the SD card.

The file system is accessible via the native FTP server running in the WiPy. 
Open your FTP client of choice and connect to:

``ftp://192.168.1.1`` ``user: micro``  ``password: python``

Boot modes
----------

If you power up normally, or press the reset button, the WiPy will boot
into standard mode: the ``boot.py`` file will be executed first, then 
``main.py`` will run.

You can override this boot sequence by pulling ``GPIO28`` **up** during reset.
The heart beat LED will flash slowly 3 times to signal that safe boot is being
requested, and then 3 more times quickly to let you know that safe boot is
going to be performed. While safe booting, the WiPy runs the factory firmware
and skips the execution of ``boot.py`` and ``main.py``. This is useful to
recover from any crash situation. 

The heart beat LED
------------------

By default the heart beat LED flashes once every 5s to signal that the system is
alive. This can be overridden through the HeartBeat class: 

``pyb.HeartBeat().disable()``

There are currently 2 kinds of errors that you might see:

1. If the heart beat LED flashes quickly, then a Python script(eg ``main.py``) 
   has an error.  Use the REPL to debug it.
2. If the heart beat LED stays on, then there was a hard fault, you cannot
   recover from this, the only way out is by pressing the reset switch.

