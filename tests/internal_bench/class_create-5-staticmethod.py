import bench


def test(num):
    for i in range(num):

        class X:
            @staticmethod
            def x():
                pass


bench.ITERS = 500_000
bench.run(test)
