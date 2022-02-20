Safe mode and factory reset
===========================

If something goes wrong with your pyboard, don't panic!  It is almost
impossible for you to break the pyboard by programming the wrong thing.

The first thing to try is to enter safe mode: this temporarily skips
execution of ``boot.py`` and ``main.py`` and gives default USB settings.

If you have problems with the filesystem you can do a factory reset,
which restores the filesystem to its original state.

Safe mode
---------

To enter safe mode, do the following steps:

1. Connect the pyboard to USB so it powers up.
2. Hold down the USR switch.
3. While still holding down USR, press and release the RST switch.
4. The LEDs will then cycle green to orange to green+orange and back again.
5. Keep holding down USR until *only the orange LED is lit*, and then let
   go of the USR switch.
6. The orange LED should flash quickly 4 times, and then turn off.
7. You are now in safe mode.

In safe mode, the ``boot.py`` and ``main.py`` files are not executed, and so
the pyboard boots up with default settings.  This means you now have access
to the filesystem (the USB drive should appear), and you can edit ``boot.py``
and ``main.py`` to fix any problems.

Entering safe mode is temporary, and does not make any changes to the
files on the pyboard.

Factory reset the filesystem
----------------------------

If you pyboard's filesystem gets corrupted (for example, you forgot to
eject/unmount it), or you have some code in ``boot.py`` or ``main.py`` which
you can't escape from, then you can reset the filesystem.

Resetting the filesystem deletes all files on the internal pyboard storage
(not the SD card), and restores the files ``boot.py``, ``main.py``, ``README.txt``
and ``pybcdc.inf`` back to their original state.

To do a factory reset of the filesystem you follow a similar procedure as
you did to enter safe mode, but release USR on green+orange:

1. Connect the pyboard to USB so it powers up.
2. Hold down the USR switch.
3. While still holding down USR, press and release the RST switch.
4. The LEDs will then cycle green to orange to green+orange and back again.
5. Keep holding down USR until *both the green and orange LEDs are lit*, and
   then let go of the USR switch.
6. The green and orange LEDs should flash quickly 4 times.
7. The red LED will turn on (so red, green and orange are now on).
8. The pyboard is now resetting the filesystem (this takes a few seconds).
9. The LEDs all turn off.
10. You now have a reset filesystem, and are in safe mode.
11. Press and release the RST switch to boot normally.
