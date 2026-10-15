try:
    import micropython, gc, weakref

    micropython.buf_contains_ptrs
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit

import sys

ptr_size = 8 if sys.maxsize > 2**32 else 4

micropython.buf_contains_ptrs(b"static")
micropython.buf_contains_ptrs(bytearray(0))

try:
    micropython.buf_contains_ptrs(1)
except TypeError:
    print("TypeError")


class A:
    pass


def store(buf, offset):
    a = A()
    addr = id(a) & ((1 << (8 * ptr_size)) - 1)
    buf[offset : offset + ptr_size] = addr.to_bytes(ptr_size, sys.byteorder)
    return weakref.ref(a)


def test(buf, mark, offset=0):
    r = store(buf, offset)
    micropython.buf_contains_ptrs(mark)
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    return r() is not None


buf = bytearray(64)
print(test(buf, buf))

buf = bytearray(256)
print(test(buf, memoryview(buf)[128:], 192))
