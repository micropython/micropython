import os
import machine, psoc6


# Try to mount the filesystem, and format the flash if it doesn't exist.
# TODO: Check "Note: the internal flash requires the programming size to be aligned to 256 bytes." and progSize parameter below !!

# Create block device instance from external or internal flash, whichever is enabled
bdev = psoc6.QSPI_Flash() if "QSPI_Flash" in dir(psoc6) else psoc6.Flash()

# sector size 4 KB for external flash
# sector size 512 B for internal flash
read_size = 0x1000 if "QSPI_Flash" in dir(psoc6) else 0x200
# page size 512 B for both flashes
write_size = 0x200

try:
    vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
    os.mount(vfs, "/")
except:
    os.VfsLfs2.mkfs(bdev, progsize=write_size, readsize=read_size)
    vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
    os.mount(vfs, "/")

if "QSPI_Flash" in dir(psoc6):
    print("\nExternal LFS2 filesystem mounted at /\n")
else:
    print("\nInternal LFS2 filesystem mounted at /\n")

if "SD_CARD" in dir(psoc6):
    bdev2 = machine.SDCARD(
        slot=1,
        width=4,
        cd="P13_5",
        cmd="P12_4",
        clk="P12_5",
        dat0="P13_0",
        dat1="P13_1",
        dat2="P13_2",
        dat3="P13_3",
    )

    # sector size 512 B
    read_size = 512
    # page size 512 B
    write_size = 512

    # create a LFS2 fs and mount it, else format and mount it
    try:
        vfs2 = os.VfsLfs2(bdev2, progsize=write_size, readsize=read_size)
        os.mount(vfs2, "/sd")
    except:
        os.VfsLfs2.mkfs(bdev2, progsize=write_size, readsize=read_size)
        vfs2 = os.VfsLfs2(bdev2, progsize=write_size, readsize=read_size)
        os.mount(vfs2, "/sd")

    print("SD card mounted at /sd\n")

    del bdev2, vfs2

del machine, os, psoc6, bdev, vfs, read_size, write_size
