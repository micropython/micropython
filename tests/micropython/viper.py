import micropython

# viper function taking and returning ints
@micropython.viper
def f(x:int, y:int) -> int:
    return x + y + 3

# viper function taking and returning objects
@micropython.viper
def g(x:object, y:object) -> object:
    return x + y

# a local (should have automatic type int)
@micropython.viper
def h(x:int) -> int:
    y = 4
    return x + y

# without type annotation, types should default to object
@micropython.viper
def i(x, y):
    return x * y

# a for loop
@micropython.viper
def viper_sum(a:int, b:int) -> int:
    total = 0
    for x in range(a, b):
        total += x
    return total

# accessing a global
@micropython.viper
def access_global():
    global gl
    gl = 1
    return gl

# this doesn't work at the moment
#@micropython.viper
#def g() -> uint:
#    return -1

print(f(1, 2))
print(g(1, 2))
print(h(3))
print(i(4, 5))
print(viper_sum(10, 10000))
print(access_global(), gl)
