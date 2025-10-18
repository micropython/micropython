try:
    import io

    io.IOBase
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit


class MyIO(io.IOBase):
    def write(self, buf):
        # CPython and MicroPython pass in different types for buf (str vs bytearray)
        print('write', len(buf))
        return len(buf)

print('test', file=MyIO())
