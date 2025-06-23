import bench


def test(num):
    class B:
        @classmethod
        def __init_subclass__(cls):
            pass

    for i in range(num):

        class X(B):
            pass


bench.ITERS = 500_000
bench.run(test)
