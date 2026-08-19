.. _uefi_intro:

Getting started with MicroPython on UEFI
========================================

This chapter covers building the UEFI image and running it, either under QEMU
(the quickest way to try it) or from the UEFI shell on real hardware.

What you get
------------

The build produces a single ``micropython.efi`` -- a PE32+ UEFI application with
its Python standard library and the :mod:`uefi` package frozen in.  It has no
external dependencies at run time: dropped onto any FAT volume a UEFI machine can
boot from, it runs a MicroPython REPL.

Building
--------

The port is built with a single ``Makefile`` in the ``ports/uefi/`` directory.
The two architectures are selected with ``ARCH``:

* ``ARCH=aa64`` (default) -- aarch64 (AAPCS64).
* ``ARCH=x64`` -- x86-64 (the Microsoft x64 ABI target).

To build the image and stage a QEMU EFI System Partition::

    make build             # aarch64 by default
    make build ARCH=x64    # x86-64

Running under QEMU
------------------

The simplest way to try the port is the interactive serial REPL under QEMU with
OVMF (x86-64) or AAVMF (aarch64) firmware::

    make run               # aarch64 serial REPL
    make run ARCH=x64      # x86-64 serial REPL

You should get the familiar MicroPython banner and a ``>>>`` prompt over the
serial console.  Try::

    >>> import sys
    >>> sys.platform
    'uefi'
    >>> help('modules')

The REPL is on the serial line rather than the graphical console, because serial
is byte-clean and handles control characters (Ctrl-C, arrow keys, paste mode)
reliably.  To bring up a real graphics window (for experimenting with the
``framebuf``/GOP framebuffer), use ``make run-gfx``.

Running on real hardware
------------------------

Copy ``micropython.efi`` to a FAT-formatted USB stick or to the EFI System
Partition, then launch it from the UEFI shell::

    FS0:
    micropython.efi

or add it as a boot option in the firmware boot manager.  A serial cable (or a
board with serial-over-USB) is the recommended way to reach the REPL; the text
console works too but is less reliable for interactive editing.

Because the application stays before ``ExitBootServices``, exiting the REPL
(Ctrl-D at the raw prompt, or ``sys.exit()``) returns control to whatever
launched it -- the shell or boot manager -- rather than powering the machine off.

The filesystem
--------------

If the application is launched from a FAT volume, that volume is auto-mounted at
``/``.  Anything you place next to ``micropython.efi`` is therefore importable
and openable::

    >>> import os
    >>> os.listdir('/')
    >>> open('/boot.py').read()

Put a ``.py`` file on the volume and ``import`` it just like on any other
MicroPython port.

Next steps
----------

* :ref:`Writing firmware scripts <uefi_scripting>` -- filesystem, timing and
  :mod:`asyncio`.
* :ref:`Firmware introspection <uefi_introspection>` -- exploring the platform
  with the :mod:`uefi` module.
