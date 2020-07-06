# tests for natively compiled functions

# basic test
@micropython.native
def native_test(x):
    print(1, [], x)


native_test(2)

# check that GC doesn't collect the native function
import gc

gc.collect()
native_test(3)

# native with 2 args
@micropython.native
def f(a, b):
    print(a + b)


f(1, 2)

# native with 3 args
@micropython.native
def f(a, b, c):
    print(a + b + c)


f(1, 2, 3)

# check not operator
@micropython.native
def f(a):
    print(not a)


f(False)
f(True)
