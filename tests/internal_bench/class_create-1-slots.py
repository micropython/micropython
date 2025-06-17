import bench


def test(num):
    l = ["x"]
    for i in range(num):

        class X:
            __slots__ = l


bench.ITERS = 500_000
bench.run(test)
