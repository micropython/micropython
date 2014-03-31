# test calling a function with *tuple and **dict

def f(a, b, c, d):
    print(a, b, c, d)

f(*(1, 2), **{'c':3, 'd':4})
f(*(1, 2), **{['c', 'd'][i]:(3 + i) for i in range(2)})

# test calling a method with *tuple and **dict

class A:
    def f(self, a, b, c, d):
        print(a, b, c, d)

a = A()
a.f(*(1, 2), **{'c':3, 'd':4})
a.f(*(1, 2), **{['c', 'd'][i]:(3 + i) for i in range(2)})
