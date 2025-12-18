import bench


def test(num):
    l = ["a", "b", "c", "d", "x"]
    for i in range(num // 40):

        class X:
            __slots__ = l


bench.run(test)
