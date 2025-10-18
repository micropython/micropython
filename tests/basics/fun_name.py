def Fun():
    pass

class A:
    def __init__(self):
        pass
    def Fun(self):
        pass

try:
    print(Fun.__name__)
    print(A.__init__.__name__)
    print(A.Fun.__name__)
    print(A().Fun.__name__)
except AttributeError:
    print('SKIP')
    raise SystemExit

# __name__ of a bound native method is not implemented in MicroPython
# the test here is to make sure it doesn't crash
try:
    str((1).to_bytes.__name__)
except AttributeError:
    pass

# name of a function that has closed over variables
# and also the name of the inner closure
def outer():
    x = 1
    def inner():
        return x
    return inner
print(outer.__name__)
print(outer().__name__)
