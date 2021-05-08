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
    # -march=x64 -mcache-lookup-bc
    0xB05: b'C\x05\x0b\x1f \x84b\xe9/\x00\x00\x00SH\x8b\x1ds\x00\x00\x00\xbe\x02\x00\x00\x00\xffS\x18\xbf\x01\x00\x00\x00H\x85\xc0u\x0cH\x8bC \xbe\x02\x00\x00\x00[\xff\xe0H\x0f\xaf\xf8H\xff\xc8\xeb\xe6ATUSH\x8b\x1dA\x00\x00\x00H\x8b\x7f\x08H\x8bk(\xff\xd5H\x8d5 \x00\x00\x00I\x89\xc4H\x8b\x05.\x00\x00\x00\x0f\xb78\xffShL\x89\xe7\xff\xd5H\x8b\x03[]A\\\xc3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x90\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x84@\x12factorial \x00\x00\r \x01"\xa1\x1c\x01\x1e\xff',
    # -march=armv7m
    0x1605: b"C\x05\x16\x1f \x84\x12\x1a\xe0\x00\x00\x13\xb5\nK\nJ{D\x9cX\x02!\xe3h\x98G\x03F\x01 3\xb9\x02!#i\x01\x93\x02\xb0\xbd\xe8\x10@\x18GXC\x01;\xf4\xe7\x00\xbfj\x00\x00\x00\x00\x00\x00\x00\xf8\xb5\tN\tK~D\xf4X@hgi\xb8G\x05F\x07K\x07I\xf2XyD\x10\x88ck\x98G(F\xb8G h\xf8\xbd6\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x1c\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x01\x84\x00\x12factorial \x00\x00\r<\x01>\xa18\x01:\xff",
}

# Populate other armv7m-derived archs based on armv7m.
for arch in (0x1A05, 0x1E05, 0x2205):
    features0_file_contents[arch] = features0_file_contents[0x1605]

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
