import bench


def test(num):
    for i in iter(range(num // 10000)):
        l = [0] * 1000
        l2 = bytearray(map(lambda x: x, l))


bench.run(test)
