# common RAM VFS
class RAMFS_RO:

    SEC_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.SEC_SIZE)

    def readblocks(self, n, buf):
        #print("readblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        for i in range(len(buf)):
            buf[i] = self.data[n * self.SEC_SIZE + i]


class RAMFS_RW(RAMFS_RO):

    def __init__(self, blocks):
        super().__init__(blocks)

    def writeblocks(self, n, buf):
        #print("writeblocks(%s, %x)" % (n, id(buf)))
        for i in range(len(buf)):
            self.data[n * self.SEC_SIZE + i] = buf[i]


class RAMFS_IOCTL_RO(RAMFS_RO):

    def __init__(self, blocks):
        super().__init__(blocks)

    def ioctl(self, op, arg):
        #print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # BP_IOCTL_SEC_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # BP_IOCTL_SEC_SIZE
            return self.SEC_SIZE


class RAMFS_IOCTL_RW(RAMFS_RW, RAMFS_IOCTL_RO):

    def __init__(self, blocks):
        super().__init__(blocks)


class RAMFS_OLD_RO(RAMFS_RO):

    def __init__(self, blocks):
        super().__init__(blocks)

    def sync(self):
        pass

    def count(self):
        return len(self.data) // self.SEC_SIZE


class RAMFS_OLD_RW(RAMFS_OLD_RO, RAMFS_RW):
    def __init__(self, blocks):
        super().__init__(blocks)
