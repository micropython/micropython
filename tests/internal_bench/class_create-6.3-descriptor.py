import bench


class D:
    def __get__(self, instance, owner=None):
        pass


def test(num):
    descriptor = D()
    for i in range(num // 40):

        class X:
            x = descriptor


bench.run(test)
