.. _pyboard_general:

General information about the pyboard
=====================================

.. contents::

Local filesystem and SD card
----------------------------

There is a small internal filesystem (a drive) on the pyboard, called ``/flash``,
which is stored within the microcontroller's flash memory.  If a micro SD card
is inserted into the slot, it is available as ``/sd``.

When the pyboard boots up, it needs to choose a filesystem to boot from.  If
there is no SD card, then it uses the internal filesystem ``/flash`` as the boot
filesystem, otherwise, it uses the SD card ``/sd``. After the boot, the current
directory is set to one of the directories above.

If needed, you can prevent the use of the SD card by creating an empty file
called ``/flash/SKIPSD``.  If this file exists when the pyboard boots
up then the SD card will be skipped and the pyboard will always boot from the
internal filesystem (in this case the SD card won't be mounted but you can still
mount and use it later in your program using ``vfs.mount``).

(Note that on older versions of the board, ``/flash`` is called ``0:/`` and ``/sd``
is called ``1:/``).

The boot filesystem is used for 2 things: it is the filesystem from which
the ``boot.py`` and ``main.py`` files are searched for, and it is the filesystem
which is made available on your PC over the USB cable.

The filesystem will be available as a USB flash drive on your PC.  You can
save files to the drive, and edit ``boot.py`` and ``main.py``.

*Remember to eject (on Linux, unmount) the USB drive before you reset your
pyboard.*

Boot modes
----------

If you power up normally, or press the reset button, the pyboard will boot
into standard mode: the ``boot.py`` file will be executed first, then the
USB will be configured, then ``main.py`` will run.

You can override this boot sequence by holding down the user switch as
the board is booting up.  Hold down user switch and press reset, and then
as you continue to hold the user switch, the LEDs will count in binary.
When the LEDs have reached the mode you want, let go of the user switch,
the LEDs for the selected mode will flash quickly, and the board will boot.

The modes are:

1. Green LED only, *standard boot*: run ``boot.py`` then ``main.py``.
2. Orange LED only, *safe boot*: don't run any scripts on boot-up.
3. Green and orange LED together, *filesystem reset*: resets the flash
   filesystem to its factory state, then boots in safe mode.

If your filesystem becomes corrupt, boot into mode 3 to fix it.
If resetting the filesystem while plugged into your compute doesn't work,
you can try doing the same procedure while the board is plugged into a USB
charger, or other USB power supply without data connection.

Errors: flashing LEDs
---------------------

There are currently 2 kinds of errors that you might see:

1. If the red and green LEDs flash alternatively, then a Python script
    (eg ``main.py``) has an error.  Use the REPL to debug it.
2. If all 4 LEDs cycle on and off slowly, then there was a hard fault.
   This cannot be recovered from and you need to do a hard reset.

Guide for using the pyboard with Windows
----------------------------------------

The following PDF guide gives information about using the pyboard with Windows,
including setting up the serial prompt and downloading new firmware using
DFU programming:
`PDF guide <http://micropython.org/resources/Micro-Python-Windows-setup.pdf>`__.

.. _hardware_index:

.. include:: hardware/index.rst
