import micropython

# viper function taking and returning ints
@micropython.viper
def f(x:int, y:int) -> int:
    return x + y + 3

# viper function taking and returning objects
@micropython.viper
def g(x:object, y:object) -> object:
    return x + y

# this doesn't work at the moment
#@micropython.viper
#def g() -> uint:
#    return -1

print(f(1, 2))
print(g(1, 2))
#print(h())
