import bench


def test(num):
    for i in range(num):

        class X:
            a = 0
            b = 0
            c = 0
            d = 0
            x = 1


bench.ITERS = 500_000
bench.run(test)
