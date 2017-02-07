General information about the WiPy
==================================

No floating point support
-------------------------

Due to space reasons, there's no floating point support, and no math module. This
means that floating point numbers cannot be used anywhere in the code, and that
all divisions must be performed using '//' instead of '/'. Example::

    >>> r = 4 // 2  # this will work
    >>> r = 4 / 2   # this WON'T

Before applying power
---------------------

.. warning::

   The GPIO pins of the WiPy are NOT 5V tolerant, connecting them to voltages higher
   than 3.6V will cause irreparable damage to the board. ADC pins, when configured
   in analog mode cannot withstand voltages above 1.8V. Keep these considerations in
   mind when wiring your electronics.

WLAN default behaviour
----------------------

When the WiPy boots with the default factory configuration starts in Access Point
mode with ``ssid`` that starts with: ``wipy-wlan`` and ``key: www.wipy.io``.
Connect to this network and the WiPy will be reachable at ``192.168.1.1``. In order
to gain access to the interactive prompt, open a telnet session to that IP address on
the default port (23). You will be asked for credentials:
``login: micro`` and ``password: python``

.. _wipy_telnet:

Telnet REPL
-----------

Linux stock telnet works like a charm (also on OSX), but other tools like putty
work quite well too. The default credentials are: **user:** ``micro``, **password:** ``python``.
See :ref:`network.server <network.server>` for info on how to change the defaults.
For instance, on a linux shell (when connected to the WiPy in AP mode)::

   $ telnet 192.168.1.1

.. _wipy_filesystem:

Local file system and FTP access
--------------------------------

There is a small internal file system (a drive) on the WiPy, called ``/flash``,
which is stored within the external serial flash memory.  If a micro SD card
is hooked-up and mounted, it will be available as well.

When the WiPy starts up, it always boots from the ``boot.py`` located in the
``/flash`` file system.

The file system is accessible via the native FTP server running in the WiPy.
Open your FTP client of choice and connect to:

**url:** ``ftp://192.168.1.1``, **user:** ``micro``, **password:** ``python``

See :ref:`network.server <network.server>` for info on how to change the defaults.
The recommended clients are: Linux stock FTP (also in OSX), Filezilla and FireFTP.
For example, on a linux shell::

   $ ftp 192.168.1.1

The FTP server on the WiPy doesn't support active mode, only passive, therefore,
if using the native unix ftp client, just after logging in do::

    ftp> passive

Besides that, the FTP server only supports one data connection at a time. Check out
the Filezilla settings section below for more info.

FileZilla settings
------------------
Do not use the quick connect button, instead, open the site manager and create a new
configuration. In the ``General`` tab make sure that encryption is set to: ``Only use
plain FTP (insecure)``. In the Transfer Settings tab limit the max number of connections
to one, otherwise FileZilla will try to open a second command connection when retrieving
and saving files, and for simplicity and to reduce code size, only one command and one
data connections are possible. Other FTP clients might behave in a similar way.

.. _wipy_firmware_upgrade:

Upgrading the firmware Over The Air
-----------------------------------

OTA software updates can be performed through the FTP server. Upload the ``mcuimg.bin`` file
to: ``/flash/sys/mcuimg.bin`` it will take around 6s. You won't see the file being stored
inside ``/flash/sys/`` because it's actually saved bypassing the user file system, so it
ends up inside the internal **hidden** file system, but rest assured that it was successfully
transferred, and it has been signed with a MD5 checksum to verify its integrity. Now, reset
the WiPy by pressing the switch on the board, or by typing::

    >>> import machine
    >>> machine.reset()

Software updates can be found in: https://github.com/wipy/wipy/releases (**Binaries.zip**).
It's always recommended to update to the latest software, but make sure to
read the **release notes** before.

.. note::

   The ``bootloader.bin`` found inside ``Binaries.zip`` is there only for reference, it's not
   needed for the Over The Air update.

In order to check your software version, do::

   >>> import os
   >>> os.uname().release

If the version number is lower than the latest release found in
`the releases <https://github.com/wipy/wipy/releases>`_, go ahead and update your WiPy!


.. _wipy_boot_modes:

Boot modes and safe boot
------------------------

If you power up normally, or press the reset button, the WiPy will boot
into standard mode; the ``boot.py`` file will be executed first, then
``main.py`` will run.

You can override this boot sequence by pulling ``GP28`` **up** (connect
it to the 3v3 output pin) during reset. This procedure also allows going
back in time to old firmware versions. The WiPy can hold up to 3 different
firmware versions, which are: the factory firmware plus 2 user updates.

After reset, if ``GP28`` is held high, the heartbeat LED will start flashing
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

On all of the above 3 scenarios, safe boot mode is entered, meaning that
the execution of both ``boot.py`` and ``main.py`` is skipped. This is
useful to recover from crash situations caused by the user scripts. The selection
made during safe boot is not persistent, therefore after the next normal reset
the latest firmware will run again.

The heartbeat LED
------------------

By default the heartbeat LED flashes once every 4s to signal that the system is
alive. This can be overridden through the :mod:`wipy` module::

   >>> import wipy
   >>> wipy.heartbeat(False)

There are currently 2 kinds of errors that you might see:

1. If the heartbeat LED flashes quickly, then a Python script (eg ``main.py``)
   has an error.  Use the REPL to debug it.
2. If the heartbeat LED stays on, then there was a hard fault, you cannot
   recover from this, the only way out is to press the reset switch.

Details on sleep modes
----------------------

* ``machine.idle()``: Power consumption: ~12mA (in WLAN STA mode). Wake sources:
  any hardware interrupt (including systick with period of 1ms), no special
  configuration required.
* ``machine.sleep()``: 950uA (in WLAN STA mode). Wake sources are ``Pin``, ``RTC``
  and ``WLAN``
* ``machine.deepsleep()``: ~350uA. Wake sources are ``Pin`` and ``RTC``.
