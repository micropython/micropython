# test calling a function with *tuple and **dict

def f(a, b, c, d):
    print(a, b, c, d)

f(*(1, 2), **{'c':3, 'd':4})
f(*(1, 2), **{['c', 'd'][i]:(3 + i) for i in range(2)})

try:
    eval("f(**{'a': 1}, *(2, 3, 4))")
except SyntaxError:
    print("SyntaxError")

# test calling a method with *tuple and **dict

class A:
    def f(self, a, b, c, d):
        print(a, b, c, d)

a = A()
a.f(*(1, 2), **{'c':3, 'd':4})
a.f(*(1, 2), **{['c', 'd'][i]:(3 + i) for i in range(2)})

try:
    eval("a.f(**{'a': 1}, *(2, 3, 4))")
except SyntaxError:
    print("SyntaxError")


# coverage test for arg allocation corner case

def f2(*args, **kwargs):
    print(len(args), len(kwargs))


f2(*iter(range(4)), **{'a': 1})

# case where *args is not a tuple/list and takes up most of the memory allocated for **kwargs
f2(*iter(range(100)), **{str(i): i for i in range(100)})

# regression test - iterable with unknown len() was exactly using preallocated
# memory causing args 4 and 5 to overflow the allocated arg array
print(1, *iter((1, 2, 3)), *iter((1, 2, 3)), 4, 5, sep=",")
