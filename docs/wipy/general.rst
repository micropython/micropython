General information about the WiPy
==================================

WLAN default behaviour
----------------------

When the WiPy boots with the default factory configuration starts in Access Point 
mode with ``ssid`` that starts with: ``wipy-wlan`` and ``key: www.wipy.io``. 
Connect to this network and the WiPy will be reachable at ``192.168.1.1``. In order
to gain access to the interactive prompt, open a telnet session to that IP address on
the default port (23). You will be asked for credentials:
``login: micro`` and ``password: python``

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

``ftp://192.168.1.1``, ``user: micro``, ``password: python``

FileZilla settings
------------------
Do not use the quick connect button, instead, open the site manager and create a new
configuration. In the ``General`` tab make sure that encryption is set to: ``Only use
plain FTP (insecure)``. In the Transfer Settings tab limit the max number of connections
to one, otherwise FileZilla will try to open a second command connection when retrieving
and saving files, and for simplicity and to reduce code size, only one command and one
data connections are possible. Other FTP clients might behave in a similar way.

Upgrading the firmware Over The Air
-----------------------------------

OTA software updates can be performed through the FTP server. Upload the ``mcuimg.bin`` file
to: ``/flash/sys/mcuimg.bin`` it will take around 6s. You won't see the file being stored
inside ``/flash/sys/`` because it's actually saved bypassing the user file system, but rest
assured that it was successfully transferred, and it has been signed with a MD5 checksum to
verify its integrity. Now, reset the MCU by pressing the switch on the board, or by typing::

    import pyb
    pyb.reset()

Boot modes
----------

If you power up normally, or press the reset button, the WiPy will boot
into standard mode; the ``boot.py`` file will be executed first, then 
``main.py`` will run.

You can override this boot sequence by pulling ``GP28`` **up** (connect
it to the 3v3 output pin) during reset. This procedure also allows going
back in time to old firmware versions. The WiPy can hold up to 3 different
firmware versions, which are: the factory firmware plus 2 user updates.

After reset, if ``GP28`` is held high, the heart beat LED will start flashing
slowly, if after 3 seconds the pin is still being held high, the LED will start
blinking a bit faster and the WiPy will select the previous user update to boot.
If the previous user update is the desired firmware image, ``GP28`` must be
released before 3 more seconds elapse. If 3 seconds later the pin is still high,
the factory firmware will be selected, the LED will flash quickly for 1.5 seconds
and the WiPy will proceed to boot. The firmware selection mechanism is as follows:


**Safe Boot Pin** ``GP28`` **released during:**

+-------------------------+-------------------------+----------------------------+
| 1st 3 secs window       | 2nd 3 secs window       | Final 1.5 secs window      |
+=========================+=========================+============================+
| | Safe boot, *latest*   | | Safe boot, *previous* | | Safe boot, the *factory* |
| | firmware is selected  | | user update selected  | | firmware is selected     |
+-------------------------+-------------------------+----------------------------+

In any if the above 3 scenarios, safe boot mode is entered, meaning that
the execution of both ``boot.py`` and ``main.py`` is skipped. This is
useful to recover from crash situations caused by the user scripts. The selection
made during safe boot is not persistent, meaning that after the next normal reset,
the latest firmware will run again.

The heart beat LED
------------------

By default the heart beat LED flashes once every 4s to signal that the system is
alive. This can be overridden through the HeartBeat class:

``pyb.HeartBeat().disable()``

There are currently 2 kinds of errors that you might see:

1. If the heart beat LED flashes quickly, then a Python script(eg ``main.py``) 
   has an error.  Use the REPL to debug it.
2. If the heart beat LED stays on, then there was a hard fault, you cannot
   recover from this, the only way out is to press the reset switch.

