# basic exceptions
x = 1
try:
    x.a()
except Exception:
    print(x)

try:
    raise IndexError
except IndexError:
    print("caught")
