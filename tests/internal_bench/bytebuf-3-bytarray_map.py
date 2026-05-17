# Doing some operation on bytearray
# No joins, but still map().
import bench


def test(num):
    for i in iter(range(num // 10000)):
        ba = bytearray(b"\0" * 1000)
        ba2 = bytearray(map(lambda x: x + 1, ba))


bench.run(test)
