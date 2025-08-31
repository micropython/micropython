import bench
import gc


class X:
    pass


def test(num):
    for i in range(num // 5):
        x = X()
    gc.collect()


bench.run(test)
