import bench


def test(num):
    for i in range(num):

        class X:
            def __new__(cls):
                pass


bench.ITERS = 500_000
bench.run(test)
