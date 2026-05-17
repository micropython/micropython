import bench


class D:
    def __set_name__(self, owner, name):
        pass


def test(num):
    descriptor = D()
    for i in range(num // 40):

        class X:
            a = descriptor
            b = descriptor
            c = descriptor
            d = descriptor
            x = descriptor


bench.run(test)
