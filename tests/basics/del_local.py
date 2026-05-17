# delete local then try to reference it
def f():
    x = 1
    y = 2
    print(x, y)
    del x
    print(y)
    try:
        print(x)
    except NameError:
        print("NameError");
f()

# delete local then try to delete it again
def g():
    x = 3
    y = 4
    print(x, y)
    del x
    print(y)
    try:
        del x
    except NameError:
        print("NameError");
g()
