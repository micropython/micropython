import bench


class X:
    __slots__ = ["x"]


def test(num):
    for i in range(num // 5):
        x = X()


bench.run(test)
