import bench


class D:
    def __set_name__(self, owner, name):
        pass


def test(num):
    descriptor = D()
    for i in range(num // 40):

        class X:
            x = descriptor


bench.run(test)
