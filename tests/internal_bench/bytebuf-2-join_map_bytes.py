# Doing some operation on bytearray
# Pretty weird way - map bytearray thru function, but make sure that
# function return bytes of size 1, then join them together. Surely,
# this is slowest way to do it.
import bench

def test(num):
    for i in iter(range(num//10000)):
        ba = bytearray(b"\0" * 1000)
        ba2 = b''.join(map(lambda x:bytes([x + 1]), ba))

bench.run(test)
