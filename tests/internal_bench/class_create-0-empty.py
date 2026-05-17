import bench


def test(num):
    for i in range(num // 40):

        class X:
            pass


bench.run(test)
