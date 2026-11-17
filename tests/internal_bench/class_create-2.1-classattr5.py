import bench


def test(num):
    for i in range(num // 40):

        class X:
            a = 0
            b = 0
            c = 0
            d = 0
            x = 1


bench.run(test)
