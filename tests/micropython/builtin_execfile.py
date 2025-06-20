# Test builtin execfile function using VFS.

try:
    import io, os, vfs

    execfile
    io.IOBase
except (ImportError, NameError, AttributeError):
    print("SKIP")
    raise SystemExit


class File(io.IOBase):
    def __init__(self, data):
        self.data = data
        self.off = 0

    def ioctl(self, request, arg):
        return 0

    def readinto(self, buf):
        buf[:] = memoryview(self.data)[self.off : self.off + len(buf)]
        self.off += len(buf)
        return len(buf)


class Filesystem:
    def __init__(self, files):
        self.files = files

    def mount(self, readonly, mkfs):
        print("mount", readonly, mkfs)

    def umount(self):
        print("umount")

    def open(self, file, mode):
        print("open", file, mode)
        if file not in self.files:
            raise OSError(2)  # ENOENT
        return File(self.files[file])


# First umount any existing mount points the target may have.
try:
    vfs.umount("/")
except OSError:
    pass
for path in os.listdir("/"):
    vfs.umount("/" + path)

# Create and mount the VFS object.
files = {
    "/test.py": "print(123)",
}
fs = Filesystem(files)
vfs.mount(fs, "/test_mnt")

# Test execfile with a file that doesn't exist.
try:
    execfile("/test_mnt/noexist.py")
except OSError:
    print("OSError")

# Test execfile with a file that does exist.
execfile("/test_mnt/test.py")

# Test that it only works with string arguments.
try:
    execfile(b"aaa")
except TypeError:
    print("TypeError")

# Unmount the VFS object.
vfs.umount(fs)
