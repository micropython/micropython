.. currentmodule:: rp2
.. _rp2.Flash:

class Flash -- access to built-in flash storage
===============================================

This class gives access to the SPI flash memory.

In most cases, to store persistent data on the device, you'll want to use a
higher-level abstraction, for example the filesystem via Python's standard file
API, but this interface is useful to :ref:`customise the filesystem
configuration <filesystem>` or implement a low-level storage system for your
application.


Constructors
------------

.. class:: Flash()

   Gets the singleton object for accessing the SPI flash memory.


Methods
-------

.. method:: Flash.readblocks(block_num, buf)
            Flash.readblocks(block_num, buf, offset)
.. method:: Flash.writeblocks(block_num, buf)
            Flash.writeblocks(block_num, buf, offset)
.. method:: Flash.ioctl(cmd, arg)

    These methods implement the simple and extended
    :ref:`block protocol <block-device-interface>` defined by
    :class:`vfs.AbstractBlockDev`.

