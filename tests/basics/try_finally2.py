# check that the Python stack does not overflow when the finally
# block itself uses more stack than the rest of the function
def f1(a, b):
    pass
def test1():
    val = 1
    try:
        raise ValueError()
    finally:
        f1(2, 2) # use some stack
        print(val) # check that the local variable is the same
try:
    test1()
except ValueError:
    pass

# same as above but with 3 args instead of 2, to use an extra stack entry
def f2(a, b, c):
    pass
def test2():
    val = 1
    try:
        raise ValueError()
    finally:
        f2(2, 2, 2) # use some stack
        print(val) # check that the local variable is the same
try:
    test2()
except ValueError:
    pass
