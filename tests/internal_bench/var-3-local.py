import bench


def test(num):
    ITERS = 20000000
    i = 0
    while i < ITERS:
        i += 1


bench.run(test)
