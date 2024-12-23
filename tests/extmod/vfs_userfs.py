# test VFS functionality with a user-defined filesystem
# also tests parts of io.IOBase implementation

import sys

try:
    import io, vfs

    io.IOBase
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class UserFile(io.IOBase):
    buffer_size = 16

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
        if req == 4:  # MP_STREAM_CLOSE
            return 0
        if req == 11:  # MP_STREAM_GET_BUFFER_SIZE
            return UserFile.buffer_size
        return -1


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
    "/usermod3.py": b"syntax error",
    "/usermod4.mpy": b"syntax error",
    "/usermod5.py": b"print('in usermod5')",
    "/usermod6.py": b"print('in usermod6')",
}
vfs.mount(UserFS(user_files), "/userfs")

# open and read a file
f = open("/userfs/data.txt")
print(f.read())

# import files from the user filesystem
sys.path.append("/userfs")
import usermod1

# import a .py file with a syntax error (file should be closed on error)
try:
    import usermod3
except SyntaxError:
    print("SyntaxError in usermod3")

# import a .mpy file with a syntax error (file should be closed on error)
try:
    import usermod4
except ValueError:
    print("ValueError in usermod4")

# Test an import with largest buffer size
UserFile.buffer_size = 255
import usermod5

# Test an import with over-size buffer size (should be safely limited internally)
UserFile.buffer_size = 1024
import usermod6

# unmount and undo path addition
vfs.umount("/userfs")
sys.path.pop()
