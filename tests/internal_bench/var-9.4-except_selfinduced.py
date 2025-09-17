import bench


class Foo:
    pass


def test(num):
    o = Foo()

    def get():
        try:
            raise AttributeError
        except AttributeError:
            return num

    i = 0
    while i < get():
        i += 1


bench.run(test)
