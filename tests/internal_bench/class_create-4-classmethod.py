import bench


def test(num):
    for i in range(num):

        class X:
            @classmethod
            def x(cls):
                pass


bench.ITERS = 500_000
bench.run(test)
