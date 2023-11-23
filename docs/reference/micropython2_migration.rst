.. _micropython2_migration:

MicroPython 2.0 Migration Guide
===============================

MicroPython 2.0 is the (currently in development, not yet available) next major
release of MicroPython.

After maintaining API compatibility for almost a decade with the ``1.x`` series, in
order to unblock some project-wide improvements MicroPython 2.0 will introduce a
small number of breaking API changes that will require some programs to be
updated. This guide explains how to update your Python code to accommodate these
changes.

This document is a work-in-progress. As more work is done on MicroPython 2.0,
more items will be added to the lists below.

**Note:** There are currently no MicroPython 2.0 firmware builds available for
download. You can build it yourself by enabling the ``MICROPY_PREVIEW_VERSION_2``
config option. As it gets closer to being ready for release, builds will be
provided for both ``1.x.y`` and ``2.0.0-preview``.

Hardware and peripherals
------------------------

Overview
~~~~~~~~

The goal is to improve consistency in the :mod:`machine` APIs across different
ports, making it easier to write code, documentation, and tutorials that work on
any supported microcontroller.

This means that some ports' APIs need to change to match other ports.

Changes
~~~~~~~

*None yet*

OS & filesystem
---------------

Overview
~~~~~~~~

The primary goal is to support the ability to execute :term:`.mpy files <.mpy
file>` directly from the filesystem without first copying them into RAM. This
improves code deployment time and reduces memory overhead and fragmentation.

Additionally, a further goal is to support a more flexible way of configuring
partitions, filesystem types, and options like USB mass storage.

Changes
~~~~~~~

*None yet*

CPython compatibility
---------------------

Overview
~~~~~~~~

The goal is to improve compatibility with CPython by removing MicroPython
extensions from CPython APIs. In most cases this means moving existing
MicroPython-specific functions or classes to new modules.

This makes it easier to write code that works on both CPython and MicroPython,
which is useful for development and testing.

Changes
~~~~~~~

Introduction of a new module :mod:`vfs`.  The following functions and
classes have moved out of :mod:`os` to :mod:`vfs`:
- `os.mount`
- `os.umount`
- `os.VfsFat`
- `os.VfsLfs1`
- `os.VfsLfs2`
- `os.VfsPosix`
