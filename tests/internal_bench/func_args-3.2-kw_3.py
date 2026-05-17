import bench


def func(a, b, c):
    pass


def test(num):
    for i in iter(range(num)):
        func(c=i, b=i, a=i)


bench.run(test)
