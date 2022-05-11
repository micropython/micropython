# test importing of .mpy files with native code (x64 only)

try:
    import usys, uio, uos

    uio.IOBase
    uos.mount
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

if not (usys.platform == "linux" and usys.maxsize > 2**32):
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


# these are the test .mpy files
# fmt: off
user_files = {
    # bad architecture
    '/mod0.mpy': b'M\x06\xfe\x00\x10',

    # test loading of viper and asm
    '/mod1.mpy': (
        b'M\x06\x0a\x1f' # header

        b'\x02' # n_qstr
        b'\x00' # n_obj

        b'\x0emod1.py\x00' # qstr0 = "mod1.py"
        b'\x0aouter\x00' # qstr1 = "outer"

        b'\x2c' # 5 bytes, have children, bytecode
            b'\x00\x02' # prelude
            b'\x01' # simple name (qstr index)
            b'\x51' # LOAD_CONST_NONE
            b'\x63' # RETURN_VALUE

        b'\x02' # 2 children

            b'\x42' # 8 bytes, no children, viper code
                b'\x00\x00\x00\x00\x00\x00' # dummy machine code
                b'\x00\x00' # slot for qstr0
                b'\x01\x0c\x0aprint\x00' # n_qstr=1, qstr0
                b'\x00' # scope_flags

            b'\x43' # 8 bytes, no children, asm code
                b'\x00\x00\x00\x00\x00\x00\x00\x00' # dummy machine code
                b'\x00\x00\x00' # scope_flags, n_pos_args, type_sig
    ),

    # test loading viper with additional scope flags and relocation
    '/mod2.mpy': (
        b'M\x06\x0a\x1f' # header

        b'\x02' # n_qstr
        b'\x00' # n_obj

        b'\x0emod2.py\x00' # qstr0 = "mod2.py"
        b'\x0aouter\x00' # qstr1 = "outer"

        b'\x2c' # 5 bytes, have children, bytecode
            b'\x00\x02' # prelude
            b'\x01' # simple name (qstr index)
            b'\x51' # LOAD_CONST_NONE
            b'\x63' # RETURN_VALUE

        b'\x01' # 1 child

            b'\x22' # 4 bytes, no children, viper code
                b'\x00\x00\x00\x00' # dummy machine code
                b'\x00' # n_qstr=0
                b'\x70' # scope_flags: VIPERBSS | VIPERRODATA | VIPERRELOC
                b'\x06\x04' # rodata=6 bytes, bss=4 bytes
                b'rodata' # rodata content
                b'\x03\x01\x00' # dummy relocation of rodata
    ),
}
# fmt: on

# create and mount a user filesystem
uos.mount(UserFS(user_files), "/userfs")
usys.path.append("/userfs")

# import .mpy files from the user filesystem
for i in range(len(user_files)):
    mod = "mod%u" % i
    try:
        __import__(mod)
        print(mod, "OK")
    except ValueError as er:
        print(mod, "ValueError", er)

# unmount and undo path addition
uos.umount("/userfs")
usys.path.pop()
