# test importing of .mpy files with native code (x64 only)

try:
    import sys, uio, uos

    uio.IOBase
    uos.mount
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

if not (sys.platform == "linux" and sys.maxsize > 2 ** 32):
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
    '/mod0.mpy': b'M\x05\xff\x00\x10',

    # test loading of viper and asm
    '/mod1.mpy': (
        b'M\x05\x0b\x1f\x20' # header

        b'\x20' # n bytes, bytecode
            b'\x00\x08\x02m\x02m' # prelude
            b'\x51' # LOAD_CONST_NONE
            b'\x63' # RETURN_VALUE

            b'\x00\x02' # n_obj, n_raw_code

        b'\x22' # n bytes, viper code
            b'\x00\x00\x00\x00\x00\x00' # dummy machine code
            b'\x00\x00' # qstr0
            b'\x01\x0c\x0aprint' # n_qstr, qstr0
            b'\x00\x00\x00' # scope_flags, n_obj, n_raw_code

        b'\x23' # n bytes, asm code
            b'\x00\x00\x00\x00\x00\x00\x00\x00' # dummy machine code
            b'\x00\x00\x00' # scope_flags, n_pos_args, type_sig
    ),

    # test loading viper with additional scope flags and relocation
    '/mod2.mpy': (
        b'M\x05\x0b\x1f\x20' # header

        b'\x20' # n bytes, bytecode
            b'\x00\x08\x02m\x02m' # prelude
            b'\x51' # LOAD_CONST_NONE
            b'\x63' # RETURN_VALUE

            b'\x00\x01' # n_obj, n_raw_code

        b'\x12' # n bytes(=4), viper code
            b'\x00\x00\x00\x00' # dummy machine code
            b'\x00' # n_qstr
            b'\x70' # scope_flags: VIPERBSS | VIPERRODATA | VIPERRELOC
            b'\x00\x00' # n_obj, n_raw_code
            b'\x06rodata' # rodata, 6 bytes
            b'\x04' # bss, 4 bytes
            b'\x03\x01\x00' # dummy relocation of rodata
    ),
}
# fmt: on

# create and mount a user filesystem
uos.mount(UserFS(user_files), "/userfs")
sys.path.append("/userfs")

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
sys.path.pop()
