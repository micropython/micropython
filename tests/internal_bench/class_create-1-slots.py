import bench


def test(num):
    l = ["x"]
    for i in range(num // 40):

        class X:
            __slots__ = l


bench.run(test)
