.. _storage_zephyr:

Filesystems and Storage
=======================

Storage modules support virtual filesystem with FAT and littlefs formats, backed by either
Zephyr DiskAccess or FlashArea (flash map) APIs depending on which the board supports.

See `vfs Filesystem Mounting <https://docs.micropython.org/en/latest/library/vfs.html?highlight=vfs#filesystem-mounting>`_.

Automatic Filesystem Mounting
------------------------------

MicroPython on Zephyr automatically attempts to mount available storage at startup:

1. **Flash storage** (if available): Automatically mounted at ``/flash`` using the
   littlefs2 filesystem. If the filesystem doesn't exist, it will be created
   automatically.

2. **Disk storage** (such as SD cards): Automatically detected and mounted at
   ``/<disk_name>`` (e.g., ``/sd`` for SD cards).

3. **Working directory**: The working directory is set to the mounted disk if
   available, otherwise to ``/flash`` if flash storage is available.

4. **Module search path**: The ``sys.path`` is automatically updated to include
   ``/<disk>/lib`` for each mounted disk and ``/flash/lib`` for flash storage.

This automatic mounting is performed by the ``_boot.py`` module at startup. For most
use cases, you don't need to manually mount filesystems.

Disk Access
-----------

The :ref:`zephyr.DiskAccess <zephyr.DiskAccess>` class can be used to access storage devices, such as SD cards.
This class uses `Zephyr Disk Access API <https://docs.zephyrproject.org/latest/reference/storage/disk/access.html>`_ and
implements the `vfs.AbstractBlockDev` protocol.

For use with SD card controllers, SD cards must be present at boot & not removed; they will
be auto detected and initialized by filesystem at boot. Use the disk driver interface and a
file system to access SD cards via disk access (see below).

The available disk names can be accessed via the ``DiskAccess.disks`` attribute, which contains
a tuple of available disk names (e.g., ``('SDHC',)`` or ``('SD',)``).

Example usage of FatFS with an SD card on the mimxrt1050_evk board::

    import vfs
    from zephyr import DiskAccess

    # List available disks
    print(DiskAccess.disks)                 # prints available disk names, e.g., ('SDHC',)

    bdev = DiskAccess('SDHC')               # create block device object using DiskAccess
    vfs.VfsFat.mkfs(bdev)                   # create FAT filesystem object using the disk storage block
    vfs.mount(bdev, '/sd')                  # mount the filesystem at the SD card subdirectory
    with open('/sd/hello.txt','w') as f:    # open a new file in the directory
        f.write('Hello world')              # write to the file
    print(open('/sd/hello.txt').read())     # print contents of the file

Note: In most cases, disks are automatically mounted at startup and manual mounting is not necessary.


Flash Area
----------

The :ref:`zephyr.FlashArea <zephyr.FlashArea>` class can be used to implement a low-level storage system or
customize filesystem configurations. To store persistent data on the device, using a higher-level filesystem
API is recommended (see below).

This class uses `Zephyr Flash map API <https://docs.zephyrproject.org/latest/reference/storage/flash_map/flash_map.html#>`_ and
implements the `vfs.AbstractBlockDev` protocol.

The available flash area names can be accessed via the ``FlashArea.areas`` dictionary, which maps partition
labels to their IDs (e.g., ``{'storage': 1, 'scratch': 4}``).

Example usage with the internal flash on the reel_board or the rv32m1_vega_ri5cy board::

    import vfs
    from zephyr import FlashArea

    # List available flash areas
    print(FlashArea.areas)                          # prints available areas, e.g., {'storage': 1, 'scratch': 4}

    bdev = FlashArea(FlashArea.areas['storage'], 4096)   # create block device object using FlashArea
    vfs.VfsLfs2.mkfs(bdev)                               # create Little filesystem object using the flash area block
    vfs.mount(bdev, '/flash')                            # mount the filesystem at the flash storage subdirectory
    with open('/flash/hello.txt','w') as f:              # open a new file in the directory
        f.write('Hello world')                           # write to the file
    print(open('/flash/hello.txt').read())               # print contents of the file

Note: In most cases, the flash storage partition is automatically mounted at ``/flash`` at startup with
automatic filesystem creation if needed, so manual mounting is not necessary.

For boards such as the frdm_k64f in which the MicroPython application spills into the default flash storage
partition, use the scratch partition by replacing ``'storage'`` with ``'scratch'``.
