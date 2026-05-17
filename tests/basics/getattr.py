# test __getattr__

class A:
    def __init__(self, d):
        self.d = d

    def __getattr__(self, attr):
        return self.d[attr]

a = A({'a':1, 'b':2})
print(a.a, a.b)

# test that any exception raised in __getattr__ propagates out
class A:
    def __getattr__(self, attr):
        if attr == "value":
            raise ValueError(123)
        else:
            raise AttributeError(456)
a = A()
try:
    a.value
except ValueError as er:
    print(er)
try:
    a.attr
except AttributeError as er:
    print(er)
