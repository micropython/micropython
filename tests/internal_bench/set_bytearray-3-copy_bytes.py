import bench


def test(num):
    buf = bytearray(16)
    i = 0
    while i < 20000000:
        buf[:] = b"UUUUUUUUUUUUUUUU"
        i += 1


bench.run(test)
