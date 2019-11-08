# Doing some operation on bytearray
# Inplace - the most memory efficient way
import bench

def test(num):
    for i in iter(range(num//10000)):
        ba = bytearray(b"\0" * 1000)
        for i in range(len(ba)):
            ba[i] += 1

bench.run(test)
