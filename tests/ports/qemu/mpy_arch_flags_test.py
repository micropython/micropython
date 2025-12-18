try:
    import sys, io, vfs

    sys.implementation._mpy
    io.IOBase
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

sys_implementation_mpy = sys.implementation._mpy
arch = (sys_implementation_mpy >> 10) & 0x0F

# RV32-only for now.
if arch != 11:
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


def mp_encode_uint(val):
    encoded = [val & 0x7F]
    val >>= 7
    while val != 0:
        encoded.insert(0, 0x80 | (val & 0x7F))
        val >>= 7
    return bytes(encoded)


def add_flags(module, flags):
    output = bytearray()
    output.extend(module[:4])
    output[2] |= 0x40
    output.extend(mp_encode_uint(flags))
    output.extend(module[4:])
    return output


arch_flags = sys_implementation_mpy >> 16
can_test_partial_flags = arch_flags not in [1 << i for i in range(16)]

# To recreate this string run:
# echo "hello=1" | mpy-cross -s module.py - | python -c 'import sys; print(sys.stdin.buffer.read())'
MPY_FILE_BASE = b"M\x06\x00\x1f\x03\x00\x12module.py\x00\x0f\nhello\x00@\x00\x02\x01\x81\x16\x02Qc"

user_files = {
    "/matching_flags.mpy": add_flags(MPY_FILE_BASE, arch_flags),
    "/invalid_flags.mpy": add_flags(MPY_FILE_BASE, (~arch_flags & 0xFFFF)),
    "/explicit_no_flags.mpy": add_flags(MPY_FILE_BASE, 0),
}
if can_test_partial_flags:
    user_files["/partial_flags_more.mpy"] = add_flags(
        MPY_FILE_BASE, (arch_flags | (arch_flags << 1)) & 0xFFFF
    )
    user_files["/partial_flags_less.mpy"] = add_flags(
        MPY_FILE_BASE, (arch_flags >> 1) & arch_flags
    )

vfs.mount(UserFS(user_files), "/userfs")
sys.path.append("/userfs")

import matching_flags

if not hasattr(matching_flags, "hello"):
    print("Improper matching_flags import")

import explicit_no_flags

if not hasattr(explicit_no_flags, "hello"):
    print("Improper explicit_no_flags import")

try:
    import invalid_flags

    print("Unexpected invalid_flags import")
except ValueError as e:
    if str(e) != "incompatible .mpy file":
        print("Unexpected invalid_flags import error:", str(e))

if can_test_partial_flags:
    import partial_flags_less

    if not hasattr(partial_flags_less, "hello"):
        print("Improper partial_flags_less import")

    try:
        import partial_flags_more

        print("Unexpected partial_flags_more import")
    except ValueError as e:
        if str(e) != "incompatible .mpy file":
            print("Unexpected partial_flags_more import error:", str(e))

print("OK")
