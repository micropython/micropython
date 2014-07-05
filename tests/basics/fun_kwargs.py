def f1(a):
    print(a)

f1(123)
f1(a=123)
try:
    f1(b=123)
except TypeError:
    print("TypeError")

def f2(a, b):
    print(a, b)

f2(1, 2)
f2(a=3, b=4)
f2(b=5, a=6)
f2(7, b=8)
try:
    f2(9, a=10)
except TypeError:
    print("TypeError")

def f3(a, b, *args):
    print(a, b, args)


f3(1, b=3)
try:
    f3(1, a=3)
except TypeError:
    print("TypeError")
try:
    f3(1, 2, 3, 4, a=5)
except TypeError:
    print("TypeError")
