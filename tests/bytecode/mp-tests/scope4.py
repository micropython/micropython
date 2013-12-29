# test scope

def f(x):
    global x42
    print(x, x42)
    x42 = x

x42 = 123
f(1)
print(x42)

x42 = 456
f(2)
print(x42)
