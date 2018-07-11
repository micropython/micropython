class A:

    var = 132

    def __init__(self):
        self.var2 = 34


a = A()
setattr(a, "var", 123)
setattr(a, "var2", 56)
print(a.var)
print(a.var2)

try:
    setattr(a, b'var3', 1)
except TypeError:
    print('TypeError')

# try setattr on a built-in function
try:
    setattr(int, 'to_bytes', 1)
except (AttributeError, TypeError):
    # uPy raises AttributeError, CPython raises TypeError
    print('AttributeError/TypeError')
