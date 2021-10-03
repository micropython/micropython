import bench


class Foo:
    def __init__(self):
        self._num = 20000000

    def num(self):
        return self._num


def test(num):
    o = Foo()
    i = 0
    while i < o.num():
        i += 1


bench.run(test)
