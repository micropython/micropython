import bench


class Foo:
    def __delattr__(self, name):  # just trigger the 'special lookups' flag on the class
        pass


def test(num):
    o = Foo()
    i = 0
    while i < getattr(o, "num", num):
        i += 1


bench.run(test)
