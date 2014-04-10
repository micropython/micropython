# test __getattr__

class A:
    def __init__(self, d):
        self.d = d

    def __getattr__(self, attr):
        return self.d[attr]

a = A({'a':1, 'b':2})
print(a.a, a.b)
