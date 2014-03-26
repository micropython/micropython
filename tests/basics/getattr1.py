class A:

    var = 132

    def __init__(self):
        self.var2 = 34

    def meth(self, i):
        return 42 + i


a = A()
print(getattr(a, "var"))
print(getattr(a, "var2"))
print(getattr(a, "meth")(5))
