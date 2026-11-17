# Test when a use IOBase class has write/readinto which returns more data than
# requested (https://github.com/micropython/micropython/issues/18845)

try:
    import io, json
except:
    print("SKIP")
    raise SystemExit


class S(io.IOBase):
    def write(self, buf):
        assert len(buf) >= 0
        return 2

    def ioctl(self, cmd, arg):
        return 0

    def readinto(self, buf):
        assert len(buf) >= 0
        return 3


try:
    print("abc", file=S())
    print("write OK")
except OSError as e:
    print("write failed, errno", e.errno)

buf = bytearray(1)
try:
    json.load(S())
    print("readinto OK")
except OSError as e:
    print("read failed, errno", e.errno)
