import bench


def test(num):
    for i in range(num // 40):

        class X:
            x = 1


bench.run(test)
