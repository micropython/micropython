import bench

from micropython import memset


def test(num):
    buf = bytearray(16)
    c = b"U"[0]
    i = 0
    while i < 20000000:
        memset(buf, 0, c)
        i += 1


bench.run(test)
