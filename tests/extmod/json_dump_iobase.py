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
        if type(buf) == memoryview:
            # MicroPython passes a memoryview, CPython passes a str
            buf = str(bytes(buf), "ascii")
        self.buf += buf
        return len(buf)


# dump to the user stream
s = S()
json.dump([123, {}], s)
print(s.buf)


# write() gets a memoryview onto the (short-lived) buffer being written, not an
# independent copy, so it can't be grown/mutated in place; MicroPython doesn't
# need to defend against this since attempting it raises TypeError like any
# other unsupported memoryview operation, rather than corrupting memory.
class MutatingS(io.IOBase):
    def write(self, buf):
        try:
            buf += buf
        except TypeError:
            print("TypeError")
        return 1


json.dump([], MutatingS())
