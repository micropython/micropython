import bench


def test(num):
    for i in range(num):

        class X:
            @property
            def x(self):
                pass


bench.ITERS = 500_000
bench.run(test)
