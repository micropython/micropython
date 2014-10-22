import micropython

# viper function taking and returning ints
@micropython.viper
def viper_int(x:int, y:int) -> int:
    return x + y + 3
print(viper_int(1, 2))

# viper function taking and returning objects
@micropython.viper
def viper_object(x:object, y:object) -> object:
    return x + y
print(viper_object(1, 2))

# a local (should have automatic type int)
@micropython.viper
def viper_local(x:int) -> int:
    y = 4
    return x + y
print(viper_local(3))

# without type annotation, types should default to object
@micropython.viper
def viper_no_annotation(x, y):
    return x * y
print(viper_no_annotation(4, 5))

# a for loop
@micropython.viper
def viper_for(a:int, b:int) -> int:
    total = 0
    for x in range(a, b):
        total += x
    return total
print(viper_for(10, 10000))

# accessing a global
@micropython.viper
def viper_access_global():
    global gl
    gl = 1
    return gl
print(viper_access_global(), gl)

# calling print with object and int types
@micropython.viper
def viper_print(x, y:int):
    print(x, y + 1)
viper_print(1, 2)

# making a tuple from an object and an int
@micropython.viper
def viper_tuple(x, y:int):
    return (x, y + 1)
print(viper_tuple(1, 2))

# making a list from an object and an int
@micropython.viper
def viper_list(x, y:int):
    return [x, y + 1]
print(viper_list(1, 2))

# making a set from an object and an int
@micropython.viper
def viper_set(x, y:int):
    return {x, y + 1}
print(sorted(list(viper_set(1, 2))))

# raising an exception
@micropython.viper
def viper_raise(x:int):
    raise OSError(x)
try:
    viper_raise(1)
except OSError as e:
    print(repr(e))

# this doesn't work at the moment
#@micropython.viper
#def g() -> uint:
#    return -1

# calling GC after defining the function
@micropython.viper
def viper_gc() -> int:
    return 1
print(viper_gc())
import gc
gc.collect()
print(viper_gc())
