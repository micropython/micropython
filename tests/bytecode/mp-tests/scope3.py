# test nested functions and scope

def f(x):
    def f2(y):
        return y + x
    print(f2(x))
    return f2
x=f(2)
print(x, x(5))
f=123
print(f(f))
