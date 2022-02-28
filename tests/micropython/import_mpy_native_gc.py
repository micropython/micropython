# Test that native code loaded from a .mpy file is retained after a GC.

try:
    import gc, sys, uio, uos

    sys.implementation.mpy
    uio.IOBase
    uos.mount
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class UserFile(uio.IOBase):
    def __init__(self, data):
        self.data = memoryview(data)
        self.pos = 0

    def readinto(self, buf):
        n = min(len(buf), len(self.data) - self.pos)
        buf[:n] = self.data[self.pos : self.pos + n]
        self.pos += n
        return n

    def ioctl(self, req, arg):
        return 0


class UserFS:
    def __init__(self, files):
        self.files = files

    def mount(self, readonly, mksfs):
        pass

    def umount(self):
        pass

    def stat(self, path):
        if path in self.files:
            return (32768, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        raise OSError

    def open(self, path, mode):
        return UserFile(self.files[path])


# Pre-compiled examples/natmod/features0 example for various architectures, keyed
# by the required value of sys.implementation.mpy.
features0_file_contents = {
    # -march=x64
    0xA06: b'M\x06\n\x1f\x01\x004build/features0.native.mpy\x00\x8aB\xe9/\x00\x00\x00SH\x8b\x1d\x83\x00\x00\x00\xbe\x02\x00\x00\x00\xffS\x18\xbf\x01\x00\x00\x00H\x85\xc0u\x0cH\x8bC \xbe\x02\x00\x00\x00[\xff\xe0H\x0f\xaf\xf8H\xff\xc8\xeb\xe6ATUSH\x8b\x1dQ\x00\x00\x00H\x8bG\x08L\x8bc(H\x8bx\x08A\xff\xd4H\x8d5+\x00\x00\x00H\x89\xc5H\x8b\x059\x00\x00\x00\x0f\xb78\xffShH\x89\xefA\xff\xd4H\x8b\x03[]A\\\xc3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x85\x00\x12factorial\x00\x10\r$\x01&\x9f \x01"\xff',
    # -march=armv7m
    0x1606: b"M\x06\x16\x1f\x01\x004build/features0.native.mpy\x00\x88B\x1a\xe0\x00\x00\x13\xb5\nK\nJ{D\x9cX\x02!\xe3h\x98G\x03F\x01 3\xb9\x02!#i\x01\x93\x02\xb0\xbd\xe8\x10@\x18GXC\x01;\xf4\xe7\x00\xbfn\x00\x00\x00\x00\x00\x00\x00\xf8\xb5\nN\nK~D\xf4XChgiXh\xb8G\x05F\x07K\x08I\xf2XyD\x10\x88ck\x98G(F\xb8G h\xf8\xbd\x00\xbf:\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x84\x00\x00\x00\x00\x00\x00\x00\x01\x84\x10\x12factorial\x00\x10\r>\x01@\x9f:\x01<\xff",
}

# Populate other armv7m-derived archs based on armv7m.
for arch in (0x1A06, 0x1E06, 0x2206):
    features0_file_contents[arch] = features0_file_contents[0x1606]

if sys.implementation.mpy not in features0_file_contents:
    print("SKIP")
    raise SystemExit

# These are the test .mpy files.
user_files = {"/features0.mpy": features0_file_contents[sys.implementation.mpy]}

# Create and mount a user filesystem.
uos.mount(UserFS(user_files), "/userfs")
sys.path.append("/userfs")

# Import the native function.
gc.collect()
from features0 import factorial

# Free the module that contained the function.
del sys.modules["features0"]

# Run a GC cycle which should reclaim the module but not the function.
gc.collect()

# Allocate lots of fragmented memory to overwrite anything that was just freed by the GC.
for i in range(1000):
    []

# Run the native function, it should not have been freed or overwritten.
print(factorial(10))

# Unmount and undo path addition.
uos.umount("/userfs")
sys.path.pop()
