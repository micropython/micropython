import bench


class Descriptor:
    def __get__(self, instance, owner=None):
        return 20000000


class Foo:
    num = Descriptor()


def test(num):
    o = Foo()
    i = 0
    while i < o.num:
        i += 1


bench.run(test)
