.. _storage_zephyr:

Filesystems and Storage
=======================

Storage modules support virtual filesystem with FAT and littlefs formats, backed by either
Zephyr DiskAccess or FlashArea (flash map) APIs depending on which the board supports.

See `vfs Filesystem Mounting <https://docs.micropython.org/en/latest/library/vfs.html?highlight=vfs#filesystem-mounting>`_.

Disk Access
-----------

The :ref:`zephyr.DiskAccess <zephyr.DiskAccess>` class can be used to access storage devices, such as SD cards.
This class uses `Zephyr Disk Access API <https://docs.zephyrproject.org/latest/reference/storage/disk/access.html>`_ and
implements the `vfs.AbstractBlockDev` protocol.

For use with SD card controllers, SD cards must be present at boot & not removed; they will
be auto detected and initialized by filesystem at boot. Use the disk driver interface and a
file system to access SD cards via disk access (see below).

Example usage of FatFS with an SD card on the mimxrt1050_evk board::

    import vfs
    from zephyr import DiskAccess
    bdev = zephyr.DiskAccess('SDHC')        # create block device object using DiskAccess
    vfs.VfsFat.mkfs(bdev)                   # create FAT filesystem object using the disk storage block
    vfs.mount(bdev, '/sd')                  # mount the filesystem at the SD card subdirectory
    with open('/sd/hello.txt','w') as f:    # open a new file in the directory
        f.write('Hello world')              # write to the file
    print(open('/sd/hello.txt').read())     # print contents of the file


Flash Area
----------

The :ref:`zephyr.FlashArea <zephyr.FlashArea>` class can be used to implement a low-level storage system or
customize filesystem configurations. To store persistent data on the device, using a higher-level filesystem
API is recommended (see below).

This class uses `Zephyr Flash map API <https://docs.zephyrproject.org/latest/reference/storage/flash_map/flash_map.html#>`_ and
implements the `vfs.AbstractBlockDev` protocol.

Example usage with the internal flash on the reel_board or the rv32m1_vega_ri5cy board::

    import vfs
    from zephyr import FlashArea
    bdev = FlashArea(FlashArea.STORAGE, 4096)   # create block device object using FlashArea
    vfs.VfsLfs2.mkfs(bdev)                      # create Little filesystem object using the flash area block
    vfs.mount(bdev, '/flash')                   # mount the filesystem at the flash storage subdirectory
    with open('/flash/hello.txt','w') as f:     # open a new file in the directory
        f.write('Hello world')                  # write to the file
    print(open('/flash/hello.txt').read())      # print contents of the file

For boards such as the frdm_k64f in which the MicroPython application spills into the default flash storage
partition, use the scratch partition by replacing ``FlashArea.STORAGE`` with the integer value 4.
