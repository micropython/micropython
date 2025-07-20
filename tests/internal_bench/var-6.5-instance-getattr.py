import bench


class Foo:
    def __getattr__(self, name):
        return 20000000


def test(num):
    o = Foo()
    i = 0
    while i < o.num:
        i += 1


bench.run(test)
