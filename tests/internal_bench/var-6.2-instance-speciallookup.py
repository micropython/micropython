import bench


class Foo:
    def __init__(self):
        self.num = 20000000

    def __delattr__(self, name):  # just trigger the 'special lookups' flag on the class
        pass


def test(num):
    o = Foo()
    i = 0
    while i < o.num:
        i += 1


bench.run(test)
