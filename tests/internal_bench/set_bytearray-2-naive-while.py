import bench


def test(num):
    buf = bytearray(16)
    c = b"U"[0]
    i = 0
    while i < 20000000:
        n = 0  # compared to -1-naive.py, eliminate the for loop
        while n < 16:
            buf[n] = c
            n += 1
        i += 1


bench.run(test)
