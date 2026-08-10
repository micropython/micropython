.. _romfs:

Working with ROMFS
==================

.. contents::

Overview
--------

ROMFS (Read-Only Memory Filesystem) is a lightweight, read-only filesystem
optimised for microcontrollers and embedded systems
where code and data need to be stored in flash memory and
accessed efficiently without being copied into RAM.

The key benefits of ROMFS are:

- **Zero-copy imports**: ``.mpy`` bytecode files stored in a ROMFS can be
  executed directly from flash memory (memory-mapped) rather than being copied
  into RAM first.  This is similar to how :ref:`frozen modules <manifest>` work,
  but does not require reflashing the entire firmware.
- **Low RAM overhead**: String and byte constant objects in ``.mpy``
  files loaded from ROMFS are referenced directly from flash, not duplicated
  in RAM.
- **Flexible deployment**: A ROMFS image can be built on a host PC and deployed
  to the device using ``mpremote``, without rebuilding the firmware.
- **Standard filesystem interface**: A ROMFS is mounted in the :ref:`VFS
  <filesystem>` and accessed via normal Python file operations (``open``,
  ``os.listdir``, ``import``, etc.).

ROMFS is complementary to both the read-write LittleFS/FAT filesystems (which
live in other flash partitions) and to :ref:`frozen modules <manifest>` (which
are compiled into the firmware itself).

.. note::

   ROMFS requires firmware that has been built with ROMFS support enabled
   (``MICROPY_VFS_ROM``).  Not all ports or boards include this by default;
   check your board's documentation or build configuration.

Port support
------------

The following ports support ROMFS.  On these ports, if a ROMFS partition is
configured for the board, it will be automatically detected at boot time and
mounted at ``/rom`` in the VFS.  Both ``/rom`` and ``/rom/lib`` are
automatically added to ``sys.path`` so that modules stored there can be
imported directly.

==============  ====================================================
Port            Notes
==============  ====================================================
alif            Supported on boards with ROMFS partition configured.
esp32           Supported with custom partition table.
esp8266         Supported on 2MiB+ boards (ESP8266_GENERIC FLASH_2M_ROMFS variant).
mimxrt          Supported on boards with ROMFS partition configured.
nrf             Supported on boards with ROMFS partition configured.
qemu            Supported (used for CI testing).
renesas-ra      Supported on boards with ROMFS partition configured.
rp2             Supported on boards with ROMFS partition configured.
samd            Supported on boards with ROMFS partition configured.
stm32           Supported on boards with ROMFS partition configured.
unix            Supported (primarily for testing).
==============  ====================================================

Enabling ROMFS for a port or board
----------------------------------

The ROMFS implementation is port specific at this time and requires too much detail to explain here.

Workflow
--------

The typical workflow for using ROMFS is:

1. Create a directory on your PC with the Python files (or ``.mpy`` files)
   you want to deploy.
2. Use ``mpremote romfs deploy <directory>`` to build and deploy the ROMFS
   image to the device.
3. The ROMFS will be mounted at ``/rom`` on next boot (or can be mounted
   immediately if the device is soft-reset).
4. Python code on the device can then ``import`` modules from the ROMFS just
   like from any other filesystem.

For example, on the host PC, with a directory "myapp/" containing app.py::

    $ mpremote romfs deploy myapp/

After a soft-reset, the device will have ``/rom/app.mpy`` available for import
(or ``/rom/app.py`` if ``mpy_cross`` is not installed).

Alternatively, you can build the ROMFS image on the host PC first, then deploy
it to the device::

    $ mpremote romfs build myapp
    $ mpremote romfs deploy myapp.romfs

See the :ref:`mpremote romfs commands <mpremote_command_romfs>` section for
full details of the ``mpremote`` commands.

Automatic mounting at boot
--------------------------

When ROMFS support is enabled in the firmware, MicroPython will automatically
attempt to mount the first ROM partition at ``/rom`` during initialisation
(after ``mp_init()``).  If the partition contains a valid ROMFS image, it is
mounted and both ``/rom`` and ``/rom/lib`` are added to ``sys.path``
automatically.

This means that after deploying a ROMFS image with ``mpremote``, a soft-reset
is sufficient to make the new modules importable.

If no valid ROMFS image is found in the partition (e.g. on a freshly-programmed
board), the mount is silently skipped.

Using mpremote to manage ROMFS
------------------------------

The :ref:`mpremote <mpremote>` ``romfs`` command can query ROMFS partitions,
build ROMFS images, and deploy images to a connected device.  See
:ref:`mpremote ROMFS commands <mpremote_command_romfs>` for command syntax,
options, and examples.

ROMFS usage examples
--------------------

Deploying a simple application
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Suppose you have a project directory ``myapp/`` with the following structure::

 ── myapp
    ├── myapp.py
    ├── utils.py
    └── lib
        └── helper.py

To deploy it to the device's ROMFS::

    $ mpremote romfs deploy myapp/
    $ mpremote tree
      tree :
      :/
      └── rom
          ├── lib
          │   └── helper.mpy
          ├── myapp.mpy
          └── utils.mpy

After a soft-reset, the modules are importable from the ROMFS as its mount point
and lib folder have been added to ``sys.path``::

    import myapp
    import utils
    import helper

Listing ROMFS contents from Python
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After mounting, the ROMFS contents can be explored like any other
filesystem::

    import os

    for entry in os.ilistdir('/rom'):
        print(entry)

    # Or simply:
    print(os.listdir('/rom'))

Manually mount a second ROMFS image
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If there are multiple ROMFS partitions, it is possible to mount
a second ROMFS image from another partition on the device.
For example, if the second ROMFS partition is at index 2, you can
mount it using::

    import vfs
    dev = vfs.rom_ioctl(2, 1)  # get second partition
    vfs.mount(vfs.VfsRom(dev), "/rom2")

Mount a ROMFS image stored in a file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A ROMFS image stored as a file within a ROMFS can be mounted as a
nested filesystem without copying it into RAM.  For example, if
``/rom/data.romfs`` exists, you can mount it using::

    # boot.py
    import vfs
    with open('/rom/data.romfs', 'rb') as f:
        fs = vfs.VfsRom(f)
    vfs.mount(fs, '/data')

Then after a soft-reset, the nested ROMFS is available at ``/data``::

    $ mpremote tree
      tree :
      :/
      ├── boot.py
      ├── data
      │   └── facts.db
      ├── main.py
      └── rom
          ├── data.romfs
          ├── lib
          │   └── helper.mpy
          ├── myapp.mpy
          └── utils.mpy

ROMFS filesystem format
=======================
ROMFS is a flexible and extensible filesystem format designed to represent a
directory hierarchy with files, where those files are read-only and their data
can be memory mapped.

Concepts:

- ``varuint`` : An unsigned integer that is encoded in a variable number of bytes.
  It is stored big-endian with the high bit of the byte set if there are following bytes.
- ``record`` : A variable sized element with a type. It is stored as two ``varuint``'s and then
  a payload. The first ``varuint`` is the record kind and the second ``varuint`` is the
  payload length (which may be zero bytes long).

A ROMFS filesystem is a record with record kind 0x14a6b1, chosen so the encoded value
is ``0xd2-0xcd-0x31`` which is ``"RM1"`` with the first two bytes having their high bit set.
If the ROMFS record's payload is non-empty then it contains records.

Record types:

- ``0`` -- **unused**: Can be used to detect corruption of the filesystem.
- ``1`` -- **padding/comments**: Can contain any data in the payload.
- ``2`` -- **verbatim data**: Used to store file data.
- ``3`` -- **indirect data**: Points to an offset within the ROMFS payload.
- ``4`` -- **directory**: The payload contains a ``varuint`` giving the length of
  the directory name in bytes, followed by the name and optional nested records
  for the directory contents (including optional metadata).
- ``5`` -- **file**: The payload contains a ``varuint`` giving the length of the
  filename in bytes, followed by the name and optional nested records.

Unknown record types are silently skipped, providing forwards compatibility.

This format is defined in ``extmod/vfs_rom.c`` in the MicroPython source.
The Python implementation used by ``mpremote`` to build images is in
``tools/mpremote/mpremote/romfs.py``.

.. seealso::

   - :ref:`filesystem` -- Overview of the MicroPython VFS and available
     filesystem types.
   - :ref:`manifest` -- How to freeze Python modules into firmware.
   - :ref:`mpy_files` -- MicroPython ``.mpy`` binary file format.
   - :ref:`mpremote` -- The full ``mpremote`` command reference.
