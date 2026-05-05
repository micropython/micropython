.. _romfs:

Working with ROMFS
==================

.. contents::

Overview
--------

ROMFS (Read-Only Memory Filesystem) is a lightweight, read-only filesystem
designed for MicroPython devices.  It is optimised for microcontrollers and
embedded systems where code and data need to be stored in flash memory and
accessed efficiently without being copied into RAM.

The key benefits of ROMFS are:

- **Zero-copy imports**: ``.mpy`` bytecode files stored in a ROMFS can be
  executed directly from flash memory (memory-mapped) rather than being copied
  into RAM first.  This is similar to how :ref:`frozen modules <manifest>` work,
  but does not require reflashing the entire firmware.
- **Low RAM overhead**: Constant objects (strings, bytes, etc.) in ``.mpy``
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
   check your board's documentation or build configuration.  Ports known to
   support ROMFS include stm32, rp2, esp32, esp8266, samd, mimxrt, nrf,
   renesas-ra, alif, and qemu.  The unix port also supports ROMFS.

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
stm32           Supported on boards with ROMFS partition configured.
rp2             Supported on boards with ROMFS partition configured.
esp32           Supported with custom partition table.
esp8266         Supported on 2MiB+ boards (ESP8266_GENERIC FLASH_2M_ROMFS variant).
samd            Supported on boards with ROMFS partition configured.
mimxrt          Supported on boards with ROMFS partition configured.
nrf             Supported on boards with ROMFS partition configured.
renesas-ra      Supported on boards with ROMFS partition configured.
alif            Supported on boards with ROMFS partition configured.
qemu            Supported (used for CI testing).
unix            Supported (primarily for testing).
==============  ====================================================

Workflow
--------

The typical workflow for using ROMFS is:

1. Create a directory on your PC with the Python files (or ``.mpy`` files)
   you want to deploy.
2. Use ``mpremote romfs deploy <directory>`` to build and deploy the ROMFS
   image to the device.
3. The ROMFS will be mounted at ``/rom`` on next boot (or can be mounted
   immediately if the device is rebooted).
4. Python code on the device can then ``import`` modules from the ROMFS just
   like from any other filesystem.

For example::

    # On the host PC, with a directory "myapp/" containing app.py:
    $ mpremote romfs deploy myapp/

After a soft-reset, the device will have ``/rom/app.py`` (or ``/rom/app.mpy``
if ``mpy_cross`` is installed) available for import.

See the :ref:`mpremote romfs commands <mpremote_command_romfs>` section for
full details of the ``mpremote`` commands.

Python API
----------

The ROMFS Python API is provided via the :mod:`vfs` module.

.. class:: vfs.VfsRom(buffer)

   Create a ROMFS filesystem object from *buffer*, which must be an object
   supporting the buffer protocol (e.g. a ``bytes``, ``bytearray``, or
   ``memoryview`` object) that contains a valid ROMFS image.

   The constructor validates that *buffer* begins with the ROMFS magic bytes
   (``b"\xd2\xcd\x31"``).  If the buffer is too small or not a valid ROMFS
   then ``OSError(ENODEV)`` is raised.

   Objects created by this constructor can be mounted using :func:`vfs.mount`.

   Example::

       import vfs

       # Load a ROMFS image from flash into a memoryview.
       dev = vfs.rom_ioctl(2, 0)   # get partition 0 as a memoryview
       fs = vfs.VfsRom(dev)
       vfs.mount(fs, '/rom')

   Or, to mount a ROMFS image stored in a file::

       import vfs

       with open('/flash/app.romfs', 'rb') as f:
           romfs_data = f.read()
       fs = vfs.VfsRom(romfs_data)
       vfs.mount(fs, '/rom2')

   The following methods are available on a ``VfsRom`` object:

   .. method:: VfsRom.open(path, mode)

       Open a file from the ROMFS.  Only read modes (``''``, ``'r'``,
       ``'rt'``, ``'rb'``) are supported.  Attempting to open a file for
       writing will raise ``OSError(EROFS)``.

       The returned file object supports ``read()``, ``seek()``,
       ``tell()``, and ``close()``.  For binary files opened in read mode,
       the returned object also supports the buffer protocol so that a
       ``memoryview`` of the file data can be obtained, which refers
       directly into the ROMFS memory (zero-copy).

   .. method:: VfsRom.ilistdir(path)

       Return an iterator over the entries in the directory *path*.  Each
       entry is a tuple ``(name, type, inode, size)`` where *type* is
       ``0x8000`` for a file or ``0x4000`` for a directory.

   .. method:: VfsRom.stat(path)

       Return an ``os.stat``-like 10-tuple for *path*.  Raises
       ``OSError(ENOENT)`` if the path does not exist.

   .. method:: VfsRom.statvfs(path)

       Return filesystem statistics.  The block size is reported as 1 and
       the block count represents the total size of the ROMFS image in bytes.
       Free blocks and free files are always 0 (read-only filesystem).

   .. method:: VfsRom.chdir(path)

       Change directory within the ROMFS.  Only the root (``'/'``) is
       supported; changing to any subdirectory raises ``OSError(EOPNOTSUPP)``.

   .. method:: VfsRom.getcwd()

       Return the current working directory within the ROMFS.  Always
       returns ``'/'``.

.. function:: vfs.rom_ioctl(op, ...)

   Low-level interface for accessing the read-only memory (ROM) partition(s)
   of the device.  This function is only available on ports that support ROMFS
   (i.e. where ``MICROPY_VFS_ROM_IOCTL`` is enabled).

   The supported operations are:

   - ``vfs.rom_ioctl(1)`` -- Return the number of available ROM partitions.
   - ``vfs.rom_ioctl(2, id)`` -- Return the ROM partition with index *id* as
     a ``memoryview`` object.  The memory can be read but not written directly.
   - ``vfs.rom_ioctl(3, id, length)`` -- Prepare a ROM partition for writing.
     Erases the first *length* bytes of the partition with index *id*.
     Returns the minimum write size in bytes (the alignment required for
     subsequent writes).
   - ``vfs.rom_ioctl(4, id, offset, buf)`` -- Write *buf* (a bytes-like object)
     to the ROM partition with index *id* at byte *offset*.
   - ``vfs.rom_ioctl(5, id)`` -- Complete a write sequence to partition *id*
     (performs any finalisation needed after writing, such as cache flushing).

   These operations are used internally by ``mpremote`` to deploy ROMFS images.
   Most users do not need to call ``vfs.rom_ioctl()`` directly.

   Example (querying available partitions)::

       import vfs

       n = vfs.rom_ioctl(1)
       print("Number of ROM partitions:", n)
       for i in range(n):
           dev = vfs.rom_ioctl(2, i)
           print(f"  Partition {i}: {len(dev)} bytes")

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

The :ref:`mpremote <mpremote>` tool provides three sub-commands for managing
ROMFS images on a connected device.

.. _mpremote_command_romfs_query:

romfs query
~~~~~~~~~~~

.. code-block:: bash

    $ mpremote romfs query

Lists all available ROMFS partitions on the device and their sizes.  Also
shows the first 12 bytes of each partition in hex and reports whether a
valid ROMFS image is present.

Example output::

    ROMFS0 partition has size 131072 bytes (32 blocks of 4096 bytes each)
      Raw contents: d2:cd:31:XX:XX:XX:XX:XX:XX:XX:XX:XX ...
      ROMFS image size: 1234

.. _mpremote_command_romfs_build:

romfs build
~~~~~~~~~~~

.. code-block:: bash

    $ mpremote romfs [-o <output>] build <source>

Build a ROMFS image from the directory *source* on the host PC.  The image
is written to *output* (default: ``<source>.romfs``).

Options:

- ``-o <output>``, ``--output <output>``: Specify the output file path.
- ``-m``, ``--mpy`` (default): Automatically compile ``.py`` files to
  ``.mpy`` using ``mpy_cross`` before adding them to the image.  Requires the
  ``mpy_cross`` Python package (``pip install mpy_cross``).
- ``--no-mpy``: Disable automatic compilation of ``.py`` files.

Example::

    $ mpremote romfs build myapp/
    Building romfs filesystem, source directory: myapp/
    /
    |-- main.py -> .mpy
    \-- lib/
        \-- helper.py -> .mpy
    Writing 2048 bytes to output file myapp.romfs

.. _mpremote_command_romfs_deploy:

romfs deploy
~~~~~~~~~~~~

.. code-block:: bash

    $ mpremote romfs [-p <partition>] deploy <source>

Deploy a ROMFS image to the device.  *source* can be either:

- A directory on the host: the ROMFS image is built in memory and deployed
  directly.
- A ``.romfs`` or ``.img`` file: the image is read from disk and deployed.

Options:

- ``-p <partition>``, ``--partition <partition>``: Specify the target
  partition index (default: ``0``).
- ``-m``, ``--mpy`` (default): Compile ``.py`` to ``.mpy`` when *source*
  is a directory.
- ``--no-mpy``: Disable automatic compilation of ``.py`` files.

After deployment, the device must be soft-reset for the new ROMFS to be
mounted at ``/rom``.

Example::

    $ mpremote romfs deploy myapp/
    Building romfs filesystem, source directory: myapp/
    /
    |-- main.py -> .mpy
    \-- lib/
        \-- helper.py -> .mpy
    Image size is 2048 bytes
    ROMFS0 partition has size 131072 bytes (32 blocks of 4096 bytes each)
    Preparing ROMFS0 partition for writing
    Deploying ROMFS to ROMFS0 partition
    ROMFS image deployed

    $ mpremote soft-reset

Examples
--------

Deploying a simple application
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Suppose you have a project directory ``myapp/`` with the following structure::

    myapp/
        main.py
        utils.py
        lib/
            helper.py

To deploy it to the device's ROMFS::

    $ mpremote romfs deploy myapp/

After a soft-reset, the modules are importable from the ROMFS::

    import main
    import utils
    from lib import helper

Listing ROMFS contents from Python
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After mounting, the ROMFS contents can be explored like any other
filesystem::

    import os

    for entry in os.ilistdir('/rom'):
        print(entry)

    # Or simply:
    print(os.listdir('/rom'))

Nesting a ROMFS within a ROMFS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A ROMFS image stored as a file within an outer ROMFS can be mounted as a
nested filesystem.  For example, if ``/rom/inner.romfs`` exists.  Because
``/rom`` is a ROMFS, file objects opened from it support the buffer protocol,
so a zero-copy ``memoryview`` can be obtained directly::

    import vfs

    with open('/rom/inner.romfs', 'rb') as f:
        inner = vfs.VfsRom(memoryview(f))
    vfs.mount(inner, '/inner')

    print(os.listdir('/inner'))

ROMFS image format
------------------

The ROMFS image format is a compact binary format designed for memory-mapped
access on microcontrollers.  A brief overview:

- The image starts with the magic bytes ``0xd2 0xcd 0x31`` (encoded as
  ``"RM1"`` with the high bits of the first two bytes set).
- The remainder of the image is composed of *records*, each with a type tag
  (varuint), a length (varuint), and a payload.
- Record types include: padding, verbatim data, indirect data pointer,
  directory, file.
- Directory and file names are stored as length-prefixed byte strings.
- File data can be stored verbatim (inline) or via an indirect pointer to
  elsewhere in the image, which enables alignment for memory-mapped access.
- Unknown record types are silently skipped, providing forwards compatibility.

This format is defined in ``extmod/vfs_rom.c`` in the MicroPython source.
The Python implementation used by ``mpremote`` to build images is in
``tools/mpremote/mpremote/romfs.py``.

.. seealso::

   :ref:`filesystem` -- Overview of the MicroPython VFS and available
   filesystem types.

   :ref:`manifest` -- How to freeze Python modules into firmware.

   :ref:`mpy_files` -- MicroPython ``.mpy`` binary file format.

   :ref:`mpremote` -- The full ``mpremote`` command reference.
