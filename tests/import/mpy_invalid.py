# test importing of invalid .mpy files

import sys, uio

try:
    uio.IOBase
    import uos
    uos.mount
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class UserFile(uio.IOBase):
    def __init__(self, data):
        self.data = data
        self.pos = 0
    def read(self):
        return self.data
    def readinto(self, buf):
        n = 0
        while n < len(buf) and self.pos < len(self.data):
            buf[n] = self.data[self.pos]
            n += 1
            self.pos += 1
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


# these are the test .mpy files
user_files = {
    '/mod0.mpy': b'', # empty file
    '/mod1.mpy': b'M', # too short header
    '/mod2.mpy': b'M\x00\x00\x00', # bad version
}

# create and mount a user filesystem
uos.mount(UserFS(user_files), '/userfs')
sys.path.append('/userfs')

# import .mpy files from the user filesystem
for i in range(len(user_files)):
    mod = 'mod%u' % i
    try:
        __import__(mod)
    except ValueError as er:
        print(mod, 'ValueError', er)

# unmount and undo path addition
uos.umount('/userfs')
sys.path.pop()
