# test importing of .mpy files with native code

try:
    import sys, io, vfs

    sys.implementation._mpy
    io.IOBase
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

mpy_arch = sys.implementation._mpy >> 8
if mpy_arch >> 2 == 0:
    # This system does not support .mpy files containing native code
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
small_int_bits = 30
valid_header = bytes([77, 6, (mpy_arch & 0x3F), small_int_bits])
# fmt: off
user_files = {
    # bad architecture (mpy_arch needed for sub-version)
    '/mod0.mpy': bytes([77, 6, 0xfc | (mpy_arch & 3), small_int_bits]),

    # test loading of viper and asm
    '/mod1.mpy': valid_header + (
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
                b'\x00\x00\x00\x00\x00\x00\x00\x00' # dummy machine code
                b'\x00' # scope_flags

            b'\x43' # 8 bytes, no children, asm code
                b'\x00\x00\x00\x00\x00\x00\x00\x00' # dummy machine code
                b'\x00\x00\x00' # scope_flags, n_pos_args, type_sig
    ),

    # test loading viper with additional scope flags and relocation
    '/mod2.mpy': valid_header + (
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
                b'\x70' # scope_flags: VIPERBSS | VIPERRODATA | VIPERRELOC
                b'\x06\x04' # rodata=6 bytes, bss=4 bytes
                b'rodata' # rodata content
                b'\x03\x01\x00' # dummy relocation of rodata
    ),
}
# fmt: on

# create and mount a user filesystem
vfs.mount(UserFS(user_files), "/userfs")
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
vfs.umount("/userfs")
sys.path.pop()
