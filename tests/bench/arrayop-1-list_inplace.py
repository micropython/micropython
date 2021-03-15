# Array operation
# Type: list, inplace operation using for. What's good about this
# method is that it doesn't require any extra memory allocation.
import bench


def test(num):
    for i in iter(range(num // 10000)):
        arr = [0] * 1000
        for i in range(len(arr)):
            arr[i] += 1


bench.run(test)
