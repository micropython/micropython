# Test deflate module with stream errors.

try:
    # Check if deflate & IOBase are available.
    import deflate, io

    io.IOBase
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Check if compression is enabled.
if not hasattr(deflate.DeflateIO, "write"):
    print("SKIP")
    raise SystemExit

formats = (deflate.RAW, deflate.ZLIB, deflate.GZIP)

# Test error on read when decompressing.


class Stream(io.IOBase):
    def readinto(self, buf):
        print("Stream.readinto", len(buf))
        return -1


try:
    deflate.DeflateIO(Stream()).read()
except OSError as er:
    print(repr(er))

# Test error on write when compressing.


class Stream(io.IOBase):
    def write(self, buf):
        print("Stream.write", buf)
        return -1


for format in formats:
    try:
        deflate.DeflateIO(Stream(), format).write("a")
    except OSError as er:
        print(repr(er))

# Test write after close.


class Stream(io.IOBase):
    def write(self, buf):
        print("Stream.write", buf)
        return -1

    def ioctl(self, cmd, arg):
        print("Stream.ioctl", cmd, arg)
        return 0


try:
    d = deflate.DeflateIO(Stream(), deflate.RAW, 0, True)
    d.close()
    d.write("a")
except OSError as er:
    print(repr(er))

# Test error on write when closing.


class Stream(io.IOBase):
    def __init__(self):
        self.num_writes = 0

    def write(self, buf):
        print("Stream.write", buf)
        if self.num_writes >= 4:
            return -1
        self.num_writes += 1
        return len(buf)


for format in formats:
    d = deflate.DeflateIO(Stream(), format)
    d.write("a")
    try:
        d.close()
    except OSError as er:
        print(repr(er))
