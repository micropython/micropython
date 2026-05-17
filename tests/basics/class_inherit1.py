class A:
    def __init__(self, x):
        print('A init', x)
        self.x = x

    def f(self):
        print(self.x, self.y)

class B(A):
    def __init__(self, x, y):
        A.__init__(self, x)
        print('B init', x, y)
        self.y = y

    def g(self):
        print(self.x, self.y)

A(1)
b = B(1, 2)
b.f()
b.g()
