import bench

class Foo:
    num = 20000000

def test(num):
    i = 0
    while i < Foo.num:
        i += 1

bench.run(test)
