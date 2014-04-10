class A:
    def __init__(self, x):
        print('A init', x)
        self.x = x

    def f(self):
        print(self.x)

    def f2(self):
        print(self.x)

class B:
    def __init__(self, x):
        print('B init', x)
        self.x = x

    def f(self):
        print(self.x)

    def f3(self):
        print(self.x)


class Sub(A, B):
    def __init__(self):
        A.__init__(self, 1)
        B.__init__(self, 2)
        print('Sub init')

    def g(self):
        print(self.x)

o = Sub()
print(o.x)
o.f()
o.f2()
o.f3()
