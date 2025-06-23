import bench


def test(num):
    for i in range(num):

        class X:
            def __getattr__(self, name):
                pass


bench.ITERS = 500_000
bench.run(test)
