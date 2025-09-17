import bench


class X:
    def __new__(cls):
        return super().__new__(cls)


def test(num):
    for i in range(num // 5):
        x = X()


bench.run(test)
