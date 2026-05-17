import bench


class Foo:
    pass


def test(num):
    o = Foo()
    o.num = num

    def get():
        try:
            return o.num
        except AttributeError:
            return num

    i = 0
    while i < get():
        i += 1


bench.run(test)
