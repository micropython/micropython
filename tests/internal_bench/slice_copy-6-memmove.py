import bench

from micropython import memmove


def test(num):
    buf = bytearray(16)
    a = b"\x00\x01\x02\x03\x04\x05\x06\x07"
    i = 0
    while i < 20000000:
        memmove(buf, 8, a, 0)  # implicit 'len'
        i += 1


bench.run(test)
