class A:
    def f(x):
        return x
    def g(y):
        def h(z):
            return x + y + z
        h(y)
A()
A.f(1)
A.g(2)(3)
