import bench


def func(a, b, c):
    pass


def test(num):
    for i in iter(range(num)):
        func(i, i, i)


bench.run(test)
