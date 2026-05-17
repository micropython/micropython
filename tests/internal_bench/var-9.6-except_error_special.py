import bench


class Foo:
    def __delattr__(self, name):  # just trigger the 'special lookups' flag on the class
        pass


def test(num):
    o = Foo()

    def get():
        try:
            return o.num
        except AttributeError:
            return num

    i = 0
    while i < get():
        i += 1


bench.run(test)
