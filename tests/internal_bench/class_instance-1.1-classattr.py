import bench


class X:
    x = 0


def test(num):
    for i in range(num // 5):
        x = X()


bench.run(test)
