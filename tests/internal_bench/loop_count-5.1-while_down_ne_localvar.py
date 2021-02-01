import bench


def test(num):
    zero = 0
    while num != zero:
        num -= 1


bench.run(test)
