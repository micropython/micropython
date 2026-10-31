# The buffer passed to a user IOBase.write method is borrowed (it may even point
# to read-only data), so the callee must not be able to resize it.  Resizing it
# (eg with `buf += buf` on the bytearray that used to be passed) corrupts memory.
# https://github.com/micropython/micropython/issues/19075

try:
    import io, json

    io.IOBase
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class S(io.IOBase):
    def write(self, buf):
        try:
            buf += buf  # must raise TypeError, not corrupt the heap
        except TypeError:
            pass
        return len(buf)


json.dump([1, 2], S())
print("no crash")
