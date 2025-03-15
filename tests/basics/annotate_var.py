# test PEP 526, variable annotations

x: int
print("x" in globals())

x: int = 1
print(x)

t: tuple = 1, 2
print(t)

# a pure annotation in a function makes that variable local
def f():
    x: int
    try:
        print(x)
    except NameError:
        print("NameError")
f()

# here, "x" should remain a global
def f():
    x.y: int
    print(x)
f()
