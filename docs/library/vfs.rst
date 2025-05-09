:mod:`vfs` -- virtual filesystem control
========================================

.. module:: vfs
   :synopsis: virtual filesystem control

The ``vfs`` module contains functions for creating filesystem objects and
mounting/unmounting them in the Virtual Filesystem.

Filesystem mounting
-------------------

Some ports provide a Virtual Filesystem (VFS) and the ability to mount multiple
"real" filesystems within this VFS.  Filesystem objects can be mounted at either
the root of the VFS, or at a subdirectory that lives in the root.  This allows
dynamic and flexible configuration of the filesystem that is seen by Python
programs.  Ports that have this functionality provide the :func:`mount` and
:func:`umount` functions, and possibly various filesystem implementations
represented by VFS classes.

.. function:: mount(fsobj, mount_point, *, readonly)

    Mount the filesystem object *fsobj* at the location in the VFS given by the
    *mount_point* string.  *fsobj* can be a a VFS object that has a ``mount()``
    method, or a block device.  If it's a block device then the filesystem type
    is automatically detected (an exception is raised if no filesystem was
    recognised).  *mount_point* may be ``'/'`` to mount *fsobj* at the root,
    or ``'/<name>'`` to mount it at a subdirectory under the root.

    If *readonly* is ``True`` then the filesystem is mounted read-only.

    During the mount process the method ``mount()`` is called on the filesystem
    object.

    Will raise ``OSError(EPERM)`` if *mount_point* is already mounted.

.. function:: mount()
    :noindex:

    With no arguments to :func:`mount`, return a list of tuples representing
    all active mountpoints.
    
    The returned list has the form *[(fsobj, mount_point), ...]*.

.. function:: umount(mount_point)

    Unmount a filesystem. *mount_point* can be a string naming the mount location,
    or a previously-mounted filesystem object.  During the unmount process the
    method ``umount()`` is called on the filesystem object.

    Will raise ``OSError(EINVAL)`` if *mount_point* is not found.

.. class:: VfsFat(block_dev)

    Create a filesystem object that uses the FAT filesystem format.  Storage of
    the FAT filesystem is provided by *block_dev*.
    Objects created by this constructor can be mounted using :func:`mount`.

    .. staticmethod:: mkfs(block_dev)

        Build a FAT filesystem on *block_dev*.

.. class:: VfsLfs1(block_dev, readsize=32, progsize=32, lookahead=32)

    Create a filesystem object that uses the `littlefs v1 filesystem format`_.
    Storage of the littlefs filesystem is provided by *block_dev*, which must
    support the :ref:`extended interface <block-device-interface>`.
    Objects created by this constructor can be mounted using :func:`mount`.

    See :ref:`filesystem` for more information.

    .. staticmethod:: mkfs(block_dev, readsize=32, progsize=32, lookahead=32)

        Build a Lfs1 filesystem on *block_dev*.

    .. note:: There are reports of littlefs v1 failing in certain situations,
              for details see `littlefs issue 347`_.

.. class:: VfsLfs2(block_dev, readsize=32, progsize=32, lookahead=32, mtime=True)

    Create a filesystem object that uses the `littlefs v2 filesystem format`_.
    Storage of the littlefs filesystem is provided by *block_dev*, which must
    support the :ref:`extended interface <block-device-interface>`.
    Objects created by this constructor can be mounted using :func:`mount`.

    The *mtime* argument enables modification timestamps for files, stored using
    littlefs attributes.  This option can be disabled or enabled differently each
    mount time and timestamps will only be added or updated if *mtime* is enabled,
    otherwise the timestamps will remain untouched.  Littlefs v2 filesystems without
    timestamps will work without reformatting and timestamps will be added
    transparently to existing files once they are opened for writing.  When *mtime*
    is enabled `os.stat` on files without timestamps will return 0 for the timestamp.

    See :ref:`filesystem` for more information.

    .. staticmethod:: mkfs(block_dev, readsize=32, progsize=32, lookahead=32)

        Build a Lfs2 filesystem on *block_dev*.

    .. note:: There are reports of littlefs v2 failing in certain situations,
              for details see `littlefs issue 295`_.

.. class:: VfsPosix(root=None)

    Create a filesystem object that accesses the host POSIX filesystem.
    If *root* is specified then it should be a path in the host filesystem to use
    as the root of the ``VfsPosix`` object.  Otherwise the current directory of
    the host filesystem is used.

.. _littlefs v1 filesystem format: https://github.com/ARMmbed/littlefs/tree/v1
.. _littlefs v2 filesystem format: https://github.com/ARMmbed/littlefs
.. _littlefs issue 295: https://github.com/ARMmbed/littlefs/issues/295
.. _littlefs issue 347: https://github.com/ARMmbed/littlefs/issues/347

Block devices
-------------

A block device is an object which implements the block protocol. This enables a
device to support MicroPython filesystems. The physical hardware is represented
by a user defined class. The :class:`AbstractBlockDev` class is a template for
the design of such a class: MicroPython does not actually provide that class,
but an actual block device class must implement the methods described below.

A concrete implementation of this class will usually allow access to the
memory-like functionality of a piece of hardware (like flash memory). A block
device can be formatted to any supported filesystem and mounted using ``os``
methods.

See :ref:`filesystem` for example implementations of block devices using the
two variants of the block protocol described below.

.. _block-device-interface:

Simple and extended interface
.............................

There are two compatible signatures for the ``readblocks`` and ``writeblocks``
methods (see below), in order to support a variety of use cases.  A given block
device may implement one form or the other, or both at the same time. The second
form (with the offset parameter) is referred to as the "extended interface".

Some filesystems (such as littlefs) that require more control over write
operations, for example writing to sub-block regions without erasing, may require
that the block device supports the extended interface.

.. class:: AbstractBlockDev(...)

    Construct a block device object.  The parameters to the constructor are
    dependent on the specific block device.

    .. method:: readblocks(block_num, buf)
                readblocks(block_num, buf, offset)

        The first form reads aligned, multiples of blocks.
        Starting at the block given by the index *block_num*, read blocks from
        the device into *buf* (an array of bytes).
        The number of blocks to read is given by the length of *buf*,
        which will be a multiple of the block size.

        The second form allows reading at arbitrary locations within a block,
        and arbitrary lengths.
        Starting at block index *block_num*, and byte offset within that block
        of *offset*, read bytes from the device into *buf* (an array of bytes).
        The number of bytes to read is given by the length of *buf*.

    .. method:: writeblocks(block_num, buf)
                writeblocks(block_num, buf, offset)

        The first form writes aligned, multiples of blocks, and requires that the
        blocks that are written to be first erased (if necessary) by this method.
        Starting at the block given by the index *block_num*, write blocks from
        *buf* (an array of bytes) to the device.
        The number of blocks to write is given by the length of *buf*,
        which will be a multiple of the block size.

        The second form allows writing at arbitrary locations within a block,
        and arbitrary lengths.  Only the bytes being written should be changed,
        and the caller of this method must ensure that the relevant blocks are
        erased via a prior ``ioctl`` call.
        Starting at block index *block_num*, and byte offset within that block
        of *offset*, write bytes from *buf* (an array of bytes) to the device.
        The number of bytes to write is given by the length of *buf*.

        Note that implementations must never implicitly erase blocks if the offset
        argument is specified, even if it is zero.

    .. method:: ioctl(op, arg)

        Control the block device and query its parameters.  The operation to
        perform is given by *op* which is one of the following integers:

          - 1 -- initialise the device (*arg* is unused)
          - 2 -- shutdown the device (*arg* is unused)
          - 3 -- sync the device (*arg* is unused)
          - 4 -- get a count of the number of blocks, should return an integer
            (*arg* is unused)
          - 5 -- get the number of bytes in a block, should return an integer,
            or ``None`` in which case the default value of 512 is used
            (*arg* is unused)
          - 6 -- erase a block, *arg* is the block number to erase

       As a minimum ``ioctl(4, ...)`` must be intercepted; for littlefs
       ``ioctl(6, ...)`` must also be intercepted. The need for others is
       hardware dependent.

       Prior to any call to ``writeblocks(block, ...)`` littlefs issues
       ``ioctl(6, block)``. This enables a device driver to erase the block
       prior to a write if the hardware requires it. Alternatively a driver
       might intercept ``ioctl(6, block)`` and return 0 (success). In this case
       the driver assumes responsibility for detecting the need for erasure.

       Unless otherwise stated ``ioctl(op, arg)`` can return ``None``.
       Consequently an implementation can ignore unused values of ``op``. Where
       ``op`` is intercepted, the return value for operations 4 and 5 are as
       detailed above. Other operations should return 0 on success and non-zero
       for failure, with the value returned being an ``OSError`` errno code.
