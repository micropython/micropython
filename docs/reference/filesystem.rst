.. _filesystem:

Working with filesystems
========================

.. contents::

This tutorial describes how MicroPython provides an on-device filesystem,
allowing standard Python file I/O methods to be used with persistent storage.

MicroPython automatically creates a default configuration and auto-detects the
primary filesystem, so this tutorial will be mostly useful if you want to modify
the partitioning, filesystem type, or use custom block devices.

The filesystem is typically backed by internal flash memory on the device, but
can also use external flash, RAM, or a custom block device.

On some ports (e.g. STM32), the filesystem may also be available over USB MSC to
a host PC. :ref:`pyboard_py` also provides a way for the host PC to access to
the filesystem on all ports.

Note: This is mainly for use on bare-metal ports like STM32 and ESP32. On ports
with an operating system (e.g. the Unix port) the filesystem is provided by the
host OS.

VFS
---

MicroPython implements a Unix-like Virtual File System (VFS) layer. All mounted
filesystems are combined into a single virtual filesystem, starting at the root
``/``. Filesystems are mounted into directories in this structure, and at
startup the working directory is changed to where the primary filesystem is
mounted.

On STM32 / Pyboard, the internal flash is mounted at ``/flash``, and optionally
the SDCard at ``/sd``. On ESP8266/ESP32, the primary filesystem is mounted at
``/``.

Block devices
-------------

A block device is an instance of a class that implements the
:class:`uos.AbstractBlockDev` protocol.

Built-in block devices
~~~~~~~~~~~~~~~~~~~~~~

Ports provide built-in block devices to access their primary flash.

On power-on, MicroPython will attempt to detect the filesystem on the default
flash and configure and mount it automatically. If no filesystem is found,
MicroPython will attempt to create a FAT filesystem spanning the entire flash.
Ports can also provide a mechanism to "factory reset" the primary flash, usually
by some combination of button presses at power on.

STM32 / Pyboard
...............

The :ref:`pyb.Flash <pyb.Flash>` class provides access to the internal flash. On some
boards which have larger external flash (e.g. Pyboard D), it will use that
instead. The ``start`` kwarg should always be specified, i.e.
``pyb.Flash(start=0)``.

Note: For backwards compatibility, when constructed with no arguments (i.e.
``pyb.Flash()``), it only implements the simple block interface and reflects the
virtual device presented to USB MSC (i.e. it includes a virtual partition table
at the start).

ESP8266
.......

The internal flash is exposed as a block device object which is created in the
``flashbdev`` module on start up. This object is by default added as a global
variable so it can usually be accessed simply as ``bdev``. This implements the
extended interface.

ESP32
.....

The :class:`esp32.Partition` class implements a block device for partitions
defined for the board. Like ESP8266, there is a global variable ``bdev`` which
points to the default partition. This implements the extended interface.

Custom block devices
~~~~~~~~~~~~~~~~~~~~

The following class implements a simple block device that stores its data in
RAM using a ``bytearray``::

    class RAMBlockDev:
        def __init__(self, block_size, num_blocks):
            self.block_size = block_size
            self.data = bytearray(block_size * num_blocks)

        def readblocks(self, block_num, buf):
            for i in range(len(buf)):
                buf[i] = self.data[block_num * self.block_size + i]

        def writeblocks(self, block_num, buf):
            for i in range(len(buf)):
                self.data[block_num * self.block_size + i] = buf[i]

        def ioctl(self, op, arg):
            if op == 4: # get number of blocks
                return len(self.data) // self.block_size
            if op == 5: # get block size
                return self.block_size

It can be used as follows::

    import os

    bdev = RAMBlockDev(512, 50)
    os.VfsFat.mkfs(bdev)
    os.mount(bdev, '/ramdisk')

An example of a block device that supports both the simple and extended
interface (i.e. both signatures and behaviours of the
:meth:`uos.AbstractBlockDev.readblocks` and
:meth:`uos.AbstractBlockDev.writeblocks` methods) is::

    class RAMBlockDev:
        def __init__(self, block_size, num_blocks):
            self.block_size = block_size
            self.data = bytearray(block_size * num_blocks)

        def readblocks(self, block_num, buf, offset=0):
            addr = block_num * self.block_size + offset
            for i in range(len(buf)):
                buf[i] = self.data[addr + i]

        def writeblocks(self, block_num, buf, offset=None):
            if offset is None:
                # do erase, then write
                for i in range(len(buf) // self.block_size):
                    self.ioctl(6, block_num + i)
                offset = 0
            addr = block_num * self.block_size + offset
            for i in range(len(buf)):
                self.data[addr + i] = buf[i]

        def ioctl(self, op, arg):
            if op == 4: # block count
                return len(self.data) // self.block_size
            if op == 5: # block size
                return self.block_size
            if op == 6: # block erase
                return 0

As it supports the extended interface, it can be used with :class:`littlefs
<uos.VfsLfs2>`::

    import os

    bdev = RAMBlockDev(512, 50)
    os.VfsLfs2.mkfs(bdev)
    os.mount(bdev, '/ramdisk')

Once mounted, the filesystem (regardless of its type) can be used as it
normally would be used from Python code, for example::

    with open('/ramdisk/hello.txt', 'w') as f:
        f.write('Hello world')
    print(open('/ramdisk/hello.txt').read())

Filesystems
-----------

MicroPython ports can provide implementations of :class:`FAT <uos.VfsFat>`,
:class:`littlefs v1 <uos.VfsLfs1>` and :class:`littlefs v2 <uos.VfsLfs2>`.

The following table shows which filesystems are included in the firmware by
default for given port/board combinations, however they can be optionally
enabled in a custom firmware build.

====================  =====  ===========  ===========
Board                 FAT    littlefs v1  littlefs v2
====================  =====  ===========  ===========
pyboard 1.0, 1.1, D   Yes    No           Yes
Other STM32           Yes    No           No
ESP8266               Yes    No           No
ESP32                 Yes    No           Yes
====================  =====  ===========  ===========

FAT
~~~

The main advantage of the FAT filesystem is that it can be accessed over USB MSC
on supported boards (e.g. STM32) without any additional drivers required on the
host PC.

However, FAT is not tolerant to power failure during writes and this can lead to
filesystem corruption. For applications that do not require USB MSC, it is
recommended to use littlefs instead.

To format the entire flash using FAT::

    # ESP8266 and ESP32
    import os
    os.umount('/')
    os.VfsFat.mkfs(bdev)
    os.mount(bdev, '/')

    # STM32
    import os, pyb
    os.umount('/flash')
    os.VfsFat.mkfs(pyb.Flash(start=0))
    os.mount(pyb.Flash(start=0), '/flash')
    os.chdir('/flash')

Littlefs
~~~~~~~~

Littlefs_ is a filesystem designed for flash-based devices, and is much more
resistant to filesystem corruption.

.. note:: There are reports of littlefs v1 and v2 failing in certain
          situations, for details see `littlefs issue 347`_  and
          `littlefs issue 295`_.

Note: It can be still be accessed over USB MSC using the `littlefs FUSE
driver`_. Note that you must use the ``-b=4096`` option to override the block
size.

.. _littlefs FUSE driver: https://github.com/ARMmbed/littlefs-fuse/tree/master/littlefs
.. _Littlefs: https://github.com/ARMmbed/littlefs
.. _littlefs issue 295: https://github.com/ARMmbed/littlefs/issues/295
.. _littlefs issue 347: https://github.com/ARMmbed/littlefs/issues/347

To format the entire flash using littlefs v2::

    # ESP8266 and ESP32
    import os
    os.umount('/')
    os.VfsLfs2.mkfs(bdev)
    os.mount(bdev, '/')

    # STM32
    import os, pyb
    os.umount('/flash')
    os.VfsLfs2.mkfs(pyb.Flash(start=0))
    os.mount(pyb.Flash(start=0), '/flash')
    os.chdir('/flash')

Hybrid (STM32)
~~~~~~~~~~~~~~

By using the ``start`` and ``len`` kwargs to :class:`pyb.Flash`, you can create
block devices spanning a subset of the flash device.

For example, to configure the first 256kiB as FAT (and available over USB MSC),
and the remainder as littlefs::

    import os, pyb
    os.umount('/flash')
    p1 = pyb.Flash(start=0, len=256*1024)
    p2 = pyb.Flash(start=256*1024)
    os.VfsFat.mkfs(p1)
    os.VfsLfs2.mkfs(p2)
    os.mount(p1, '/flash')
    os.mount(p2, '/data')
    os.chdir('/flash')

This might be useful to make your Python files, configuration and other
rarely-modified content available over USB MSC, but allowing for frequently
changing application data to reside on littlefs with better resilience to power
failure, etc.

The partition at offset ``0`` will be mounted automatically (and the filesystem
type automatically detected), but you can add::

    import os, pyb
    p2 = pyb.Flash(start=256*1024)
    os.mount(p2, '/data')

to ``boot.py`` to mount the data partition.

Hybrid (ESP32)
~~~~~~~~~~~~~~

On ESP32, if you build custom firmware, you can modify ``partitions.csv`` to
define an arbitrary partition layout.

At boot, the partition named "vfs" will be mounted at ``/`` by default, but any
additional partitions can be mounted in your ``boot.py`` using::

    import esp32, os
    p = esp32.Partition.find(esp32.Partition.TYPE_DATA, label='foo')
    os.mount(p, '/foo')

