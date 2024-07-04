import os
import machine, psoc6

if "SD_CARD" in dir(psoc6):
    bdev2 = machine.SDCard(
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

    print("SD card mounted at /sd")

    del bdev2, vfs2
