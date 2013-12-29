# test scope

def f(x):
    def f2(y):
        print(y, x42, y42)
    x42 = x = y42 = 123
    myf2 = f2
    x42 = 456
    return myf2

myf = f(1)
myf(1)
