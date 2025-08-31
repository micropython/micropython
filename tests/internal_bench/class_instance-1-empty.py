import bench


class X:
    pass


def test(num):
    for i in range(num // 5):
        x = X()


bench.run(test)
