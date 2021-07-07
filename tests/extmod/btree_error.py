# Test that errno's propagate correctly through btree module.

try:
    import btree, uio, uerrno

    uio.IOBase
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class Device(uio.IOBase):
    def __init__(self, read_ret=0, ioctl_ret=0):
        self.read_ret = read_ret
        self.ioctl_ret = ioctl_ret

    def readinto(self, buf):
        print("read", len(buf))
        return self.read_ret

    def ioctl(self, cmd, arg):
        print("ioctl", cmd)
        return self.ioctl_ret


# Invalid pagesize; errno comes from btree library
try:
    db = btree.open(Device(), pagesize=511)
except OSError as er:
    print("OSError", er.errno == uerrno.EINVAL)

# Valid pagesize, device returns error on read; errno comes from Device.readinto
try:
    db = btree.open(Device(-1000), pagesize=512)
except OSError as er:
    print(repr(er))

# Valid pagesize, device returns error on seek; errno comes from Device.ioctl
try:
    db = btree.open(Device(0, -1001), pagesize=512)
except OSError as er:
    print(repr(er))
