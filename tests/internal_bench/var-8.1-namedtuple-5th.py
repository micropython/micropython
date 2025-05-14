import bench
from collections import namedtuple

T = namedtuple("Tup", ["foo1", "foo2", "foo3", "foo4", "num"])


def test(num):
    t = T(0, 0, 0, 0, 20000000)
    i = 0
    while i < t.num:
        i += 1


bench.run(test)
