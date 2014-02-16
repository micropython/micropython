# function with just varargs
def f1(*args):
    print(args)

f1()
f1(1)
f1(1, 2)

# function with 1 arg, then varargs
def f2(a, *args):
    print(a, args)

f2(1)
f2(1, 2)
f2(1, 2, 3)

# function with 2 args, then varargs
def f3(a, b, *args):
    print(a, b, args)

f3(1, 2)
f3(1, 2, 3)
f3(1, 2, 3, 4)

# function with 1 default arg, then varargs
def f4(a=0, *args):
    print(a, args)

f4()
f4(1)
f4(1, 2)
f4(1, 2, 3)

# function with 1 arg, 1 default arg, then varargs
def f5(a, b=0, *args):
    print(a, b, args)

f5(1)
f5(1, 2)
f5(1, 2, 3)
f5(1, 2, 3, 4)
