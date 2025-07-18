import bench


class D:
    def __set_name__(self, owner, name):
        pass


def test(num):
    descriptor = D()
    for i in range(num):

        class X:
            x = descriptor


bench.ITERS = 500_000
bench.run(test)
