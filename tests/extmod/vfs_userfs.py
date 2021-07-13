# test VFS functionality with a user-defined filesystem
# also tests parts of uio.IOBase implementation

import sys

try:
    import uio

    uio.IOBase
    import uos

    uos.mount
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class UserFile(uio.IOBase):
    def __init__(self, mode, data):
        assert isinstance(data, bytes)
        self.is_text = mode.find("b") == -1
        self.data = data
        self.pos = 0

    def read(self):
        if self.is_text:
            return str(self.data, "utf8")
        else:
            return self.data

    def readinto(self, buf):
        assert not self.is_text
        n = 0
        while n < len(buf) and self.pos < len(self.data):
            buf[n] = self.data[self.pos]
            n += 1
            self.pos += 1
        return n

    def ioctl(self, req, arg):
        print("ioctl", req, arg)
        return 0


class UserFS:
    def __init__(self, files):
        self.files = files

    def mount(self, readonly, mksfs):
        pass

    def umount(self):
        pass

    def stat(self, path):
        print("stat", path)
        if path in self.files:
            return (32768, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        raise OSError

    def open(self, path, mode):
        print("open", path, mode)
        return UserFile(mode, self.files[path])


# create and mount a user filesystem
user_files = {
    "/data.txt": b"some data in a text file",
    "/usermod1.py": b"print('in usermod1')\nimport usermod2",
    "/usermod2.py": b"print('in usermod2')",
}
uos.mount(UserFS(user_files), "/userfs")

# open and read a file
f = open("/userfs/data.txt")
print(f.read())

# import files from the user filesystem
sys.path.append("/userfs")
import usermod1

# unmount and undo path addition
uos.umount("/userfs")
sys.path.pop()
