# test calling a function with keywords given by **dict

def f(a, b):
    print(a, b)

f(1, **{'b':2})
f(1, **{'b':val for val in range(1)})

try:
    f(1, **{len:2})
except TypeError:
    print('TypeError')

# test calling a method with keywords given by **dict

class A:
    def f(self, a, b):
        print(a, b)

a = A()
a.f(1, **{'b':2})
a.f(1, **{'b':val for val in range(1)})

try:
    f(1, **{len: 1})
except TypeError:
    print(True)
else:
    print(False)
