# tests basics of bound methods

# uPy and CPython differ when printing a bound method, so just print the type
print(type(repr([].append)))

class A:
    def f(self):
        return 0
    def g(self, a):
        return a
    def h(self, a, b, c, d, e, f):
        return a + b + c + d + e + f

# bound method with no extra args
m = A().f
print(m())

# bound method with 1 extra arg
m = A().g
print(m(1))

# bound method with lots of extra args
m = A().h
print(m(1, 2, 3, 4, 5, 6))
