import bench


def test(num):
    buf = bytearray(16)
    c = b"U"[0]
    i = 0
    while i < 20000000:
        for n in range(len(buf)):
            buf[n] = c
        i += 1


bench.run(test)
