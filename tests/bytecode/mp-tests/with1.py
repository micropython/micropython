with x:
    f()
with x():
    f()
with f() as x:
    f(x)
with f() as x, g() as y:
    f(x, y)
