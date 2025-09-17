import bench


class X:
    def __init__(self):
        pass


def test(num):
    for i in range(num // 5):
        x = X()


bench.run(test)
