import bench
from ucollections import namedtuple

T = namedtuple("Tup", ["num", "bar"])


def test(num):
    t = T(20000000, 0)
    i = 0
    while i < t.num:
        i += 1


bench.run(test)
