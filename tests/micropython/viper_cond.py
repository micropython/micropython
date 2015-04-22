# using False as a conditional
@micropython.viper
def f():
    x = False
    if x:
        pass
    else:
        print("not x", x)
f()

# using True as a conditional
@micropython.viper
def f():
    x = True
    if x:
        print("x", x)
f()

# using an int as a conditional
@micropython.viper
def g():
    y = 1
    if y:
        print("y", y)
g()
