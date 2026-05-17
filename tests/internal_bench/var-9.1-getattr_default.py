import bench


class Foo:
    pass


def test(num):
    o = Foo()
    i = 0
    while i < getattr(o, "num", num):
        i += 1


bench.run(test)
