import bench

class Foo:

    def __init__(self):
        self.num1 = 0
        self.num2 = 0
        self.num3 = 0
        self.num4 = 0
        self.num = 20000000

def test(num):
    o = Foo()
    i = 0
    while i < o.num:
        i += 1

bench.run(test)
