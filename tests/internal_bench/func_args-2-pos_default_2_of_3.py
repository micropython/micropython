import bench


def func(a, b=1, c=2):
    pass


def test(num):
    for i in iter(range(num)):
        func(i)


bench.run(test)
