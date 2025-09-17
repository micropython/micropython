import bench

X = object


def test(num):
    for i in range(num // 5):
        x = X()


bench.run(test)
