import bench


def test(num):
    i = 0
    while i < num:
        i += 1


bench.run(test)
