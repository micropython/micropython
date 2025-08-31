import bench


class X:
    def __init__(self):
        return super().__init__()


def test(num):
    for i in range(num // 5):
        x = X()


bench.run(test)
