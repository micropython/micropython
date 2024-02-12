# Test that native code loaded from a .mpy file is retained after a GC.

try:
    import gc, sys, io, vfs

    sys.implementation._mpy
    io.IOBase
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class UserFile(io.IOBase):
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
# by the required value of sys.implementation._mpy (without sub-version).
# cd examples/natmod/features0
# make clean
# make ARCH=x64 # or ARCH=armv6m
# cat features0.mpy | python -c 'import sys; print(sys.stdin.buffer.read())'
features0_file_contents = {
    # -march=x64
    0x806: b'M\x06\x0a\x1f\x02\x004build/features0.native.mpy\x00\x12factorial\x00\x8a\x02\xe9/\x00\x00\x00SH\x8b\x1d\x83\x00\x00\x00\xbe\x02\x00\x00\x00\xffS\x18\xbf\x01\x00\x00\x00H\x85\xc0u\x0cH\x8bC \xbe\x02\x00\x00\x00[\xff\xe0H\x0f\xaf\xf8H\xff\xc8\xeb\xe6ATUSH\x8b\x1dQ\x00\x00\x00H\x8bG\x08L\x8bc(H\x8bx\x08A\xff\xd4H\x8d5+\x00\x00\x00H\x89\xc5H\x8b\x059\x00\x00\x00\x0f\xb7x\x02\xffShH\x89\xefA\xff\xd4H\x8b\x03[]A\\\xc3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x11$\r&\xa3 \x01"\xff',
    # -march=armv6m
    0x1006: b"M\x06\x12\x1f\x02\x004build/features0.native.mpy\x00\x12factorial\x00\x88\x02\x18\xe0\x00\x00\x10\xb5\tK\tJ{D\x9cX\x02!\xe3h\x98G\x03\x00\x01 \x00+\x02\xd0XC\x01;\xfa\xe7\x02!#i\x98G\x10\xbd\xc0Fj\x00\x00\x00\x00\x00\x00\x00\xf8\xb5\nN\nK~D\xf4XChgiXh\xb8G\x05\x00\x07K\x08I\xf3XyDX\x88ck\x98G(\x00\xb8G h\xf8\xbd\xc0F:\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x11<\r>\xa38\x01:\xff",
}

# Populate armv7m-derived archs based on armv6m.
for arch in (0x1406, 0x1806, 0x1C06, 0x2006):
    features0_file_contents[arch] = features0_file_contents[0x1006]

# Check that a .mpy exists for the target (ignore sub-version in lookup).
sys_implementation_mpy = sys.implementation._mpy & ~(3 << 8)
if sys_implementation_mpy not in features0_file_contents:
    print("SKIP")
    raise SystemExit

# These are the test .mpy files.
user_files = {"/features0.mpy": features0_file_contents[sys_implementation_mpy]}

# Create and mount a user filesystem.
vfs.mount(UserFS(user_files), "/userfs")
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
vfs.umount("/userfs")
sys.path.pop()
