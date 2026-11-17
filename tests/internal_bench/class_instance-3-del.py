import bench


class X:
    def __del__(self):
        pass


def test(num):
    for i in range(num // 5):
        x = X()


bench.run(test)
