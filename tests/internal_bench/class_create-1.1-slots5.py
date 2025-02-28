import bench


def test(num):
    l = ["a", "b", "c", "d", "x"]
    for i in range(num):

        class X:
            __slots__ = l


bench.ITERS = 500_000
bench.run(test)
