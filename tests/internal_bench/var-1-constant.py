import bench


def test(num):
    i = 0
    while i < 20000000:
        i += 1


bench.run(test)
