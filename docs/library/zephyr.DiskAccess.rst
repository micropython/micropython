.. currentmodule:: zephyr
.. _zephyr.DiskAccess:

class DiskAccess -- access to disk storage
==========================================

Uses `Zephyr Disk Access API <https://docs.zephyrproject.org/latest/reference/storage/disk/access.html>`_.

This class allows access to storage devices on the board, such as support for SD card controllers and
interfacing with SD cards via SPI. Disk devices are automatically detected and initialized on boot using
Zephyr devicetree data.

The Zephyr disk access class enables the transfer of data between a disk device and an accessible memory buffer given a disk name,
buffer, starting disk block, and number of sectors to read. MicroPython reads as many blocks as necessary to fill the buffer, so
the number of sectors to read is found by dividing the buffer length by block size of the disk.

Constructors
------------

.. class:: DiskAccess(disk_name)

   Gets an object for accessing disk memory of the specific disk.
   For accessing an SD card on the mimxrt1050_evk, ``disk_name`` would be ``SDHC``. See board documentation and
   devicetree for usable disk names for your board (ex. RT boards use style USDHC#).

Methods
-------

.. method:: DiskAccess.readblocks(block_num, buf)
            DiskAccess.readblocks(block_num, buf, offset)
.. method:: DiskAccess.writeblocks(block_num, buf)
            DiskAccess.writeblocks(block_num, buf, offset)
.. method:: DiskAccess.ioctl(cmd, arg)

    These methods implement the simple and extended
    :ref:`block protocol <block-device-interface>` defined by
    :class:`vfs.AbstractBlockDev`.

