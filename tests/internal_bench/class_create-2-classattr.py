import bench


def test(num):
    for i in range(num):

        class X:
            x = 1


bench.ITERS = 500_000
bench.run(test)
