Reset and boot modes
====================

There are soft resets and hard resets. 

   - A soft reset simply clears the state of the MicroPython virtual machine, 
     but leaves hardware peripherals unaffected. To do a soft reset, simply press 
     **Ctrl+D** on the REPL, or within a script do::

        import sys
        sys.exit()

   - A hard reset is the same as performing a power cycle to the board. In order to
     hard reset the WiPy, press the switch on the board or::

        import machine
        machine.reset()

Safe boot
---------

If something goes wrong with your WiPy, don't panic!  It is almost
impossible for you to break the WiPy by programming the wrong thing.

The first thing to try is to boot in safe mode: this temporarily skips
execution of ``boot.py`` and ``main.py`` and gives default WLAN settings.

If you have problems with the filesystem you can :ref:`format the internal flash
drive <wipy_factory_reset>`.

To boot in safe mode, follow the detailed instructions described :ref:`here <wipy_boot_modes>`.

In safe mode, the ``boot.py`` and ``main.py`` files are not executed, and so
the WiPy boots up with default settings.  This means you now have access
to the filesystem, and you can edit ``boot.py`` and ``main.py`` to fix any problems.

Entering safe mode is temporary, and does not make any changes to the
files on the WiPy.

.. _wipy_factory_reset:

Factory reset the filesystem
----------------------------

If you WiPy's filesystem gets corrupted (very unlikely, but possible), you
can format it very easily by doing::

   >>> import os
   >>> os.mkfs('/flash')

Resetting the filesystem deletes all files on the internal WiPy storage
(not the SD card), and restores the files ``boot.py`` and ``main.py`` back
to their original state after the next reset.

