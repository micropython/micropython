# test importing of invalid .mpy files

try:
    import sys, io, vfs

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
        if req == 4:  # MP_STREAM_CLOSE
            return 0
        return -1


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


# these are the test .mpy files
user_files = {
    "/mod0.mpy": b"",  # empty file
    "/mod1.mpy": b"M",  # too short header
    "/mod2.mpy": b"M\x00\x00\x00",  # bad version
}

# create and mount a user filesystem
vfs.mount(UserFS(user_files), "/userfs")
sys.path.append("/userfs")

# import .mpy files from the user filesystem
for i in range(len(user_files)):
    mod = "mod%u" % i
    try:
        __import__(mod)
    except ValueError as er:
        print(mod, "ValueError", er)

# unmount and undo path addition
vfs.umount("/userfs")
sys.path.pop()
