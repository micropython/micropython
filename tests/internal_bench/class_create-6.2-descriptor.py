import bench


class D:
    def __get__(self, instance, owner=None):
        pass


def test(num):
    descriptor = D()
    for i in range(num):

        class X:
            x = descriptor


bench.ITERS = 500_000
bench.run(test)
