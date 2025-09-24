# test json.dump in combination with io.IOBase

try:
    import io, json
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(io, "IOBase"):
    print("SKIP")
    raise SystemExit


# a user stream that only has the write method
class S(io.IOBase):
    def __init__(self):
        self.buf = ""

    def write(self, buf):
        if type(buf) == bytearray:
            # MicroPython passes a bytearray, CPython passes a str
            buf = str(buf, "ascii")
        self.buf += buf
        return len(buf)


# dump to the user stream
s = S()
json.dump([123, {}], s)
print(s.buf)
