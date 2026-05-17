import bench
import gc

X = object


def test(num):
    for i in range(num // 5):
        x = X()
    gc.collect()


bench.run(test)
