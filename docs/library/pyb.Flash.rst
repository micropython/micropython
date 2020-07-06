.. currentmodule:: pyb
.. _pyb.Flash:

class Flash -- access to built-in flash storage
===============================================

The Flash class allows direct access to the primary flash device on the pyboard.

In most cases, to store persistent data on the device, you'll want to use a
higher-level abstraction, for example the filesystem via Python's standard file
API, but this interface is useful to :ref:`customise the filesystem
configuration <filesystem>` or implement a low-level storage system for your
application.

Constructors
------------

.. class:: pyb.Flash()

   Create and return a block device that represents the flash device presented
   to the USB mass storage interface.

   It includes a virtual partition table at the start, and the actual flash
   starts at block ``0x100``.

   This constructor is deprecated and will be removed in a future version of MicroPython.

.. class:: pyb.Flash(\*, start=-1, len=-1)
   :noindex:

   Create and return a block device that accesses the flash at the specified offset. The length defaults to the remaining size of the device.

   The *start* and *len* offsets are in bytes, and must be a multiple of the block size (typically 512 for internal flash).

Methods
-------

.. method:: Flash.readblocks(block_num, buf)
            Flash.readblocks(block_num, buf, offset)
.. method:: Flash.writeblocks(block_num, buf)
            Flash.writeblocks(block_num, buf, offset)
.. method:: Flash.ioctl(cmd, arg)

    These methods implement the simple and :ref:`extended
    <block-device-interface>` block protocol defined by
    :class:`uos.AbstractBlockDev`.

Hardware Note
-------------

On boards with external spiflash (e.g. Pyboard D), the MicroPython firmware will
be configured to use that as the primary flash storage. On all other boards, the
internal flash inside the :term:`MCU` will be used.
