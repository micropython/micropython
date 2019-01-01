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

# __name__ of a bound native method is not implemented in uPy
# the test here is to make sure it doesn't crash
try:
    str((1).to_bytes.__name__)
except AttributeError:
    pass
