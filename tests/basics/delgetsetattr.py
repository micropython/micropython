# test __delattr__ ,__getattr__ and __setattr__

class A:
    def __init__(self, d):
        self.d = d

    def __getattr__(self, attr):
        if  attr in self.d:
            return self.d[attr]
        else:
            raise AttributeError(attr)

    def __setattr__(self, attr, value):
        super(A, self).__setattr__(attr, value)

    def __delattr__(self, attr):
        del self.d[attr]

a = A({'a':1, 'b':2})
print(a.a, a.b)
try:
    del a.a
except AttributeError:
    print("AttributeError")
try:
    print(a.a)
except AttributeError:
    print("AttributeError")