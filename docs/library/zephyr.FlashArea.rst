.. currentmodule:: zephyr
.. _zephyr.FlashArea:

class FlashArea -- access to built-in flash storage
===================================================

Uses `Zephyr flash map API <https://docs.zephyrproject.org/latest/reference/storage/flash_map/flash_map.html#flash-map>`_.

This class allows access to device flash partition data.
Flash area structs consist of a globally unique ID number, the name of the flash device the partition is in,
the start offset (expressed in relation to the flash memory beginning address per partition),
and the size of the partition that the device represents. For fixed flash partitions, data from the device
tree is used; however, fixed flash partitioning is not enforced in MicroPython because MCUBoot is not enabled.

Constructors
------------

.. class:: FlashArea(id, block_size)

   Gets an object for accessing flash memory at partition specified by ``id`` and with block size of ``block_size``.

   ``id`` values are integers correlating to fixed flash partitions defined in the devicetree.
   A commonly used partition is the designated flash storage area defined as ``FlashArea.STORAGE`` if
   ``FLASH_AREA_LABEL_EXISTS(storage)`` returns true at boot.
   Zephyr devicetree fixed flash partitions are ``boot_partition``, ``slot0_partition``, ``slot1_partition``, and
   ``scratch_partition``. Because MCUBoot is not enabled by default for MicroPython, these fixed partitions can be accessed by
   ID integer values 1, 2, 3, and 4, respectively.

Methods
-------

.. method:: FlashArea.readblocks(block_num, buf)
            FlashArea.readblocks(block_num, buf, offset)
.. method:: FlashArea.writeblocks(block_num, buf)
            FlashArea.writeblocks(block_num, buf, offset)
.. method:: FlashArea.ioctl(cmd, arg)

    These methods implement the simple and extended
    :ref:`block protocol <block-device-interface>` defined by
    :class:`vfs.AbstractBlockDev`.
