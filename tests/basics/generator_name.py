# test __name__ on generator functions

def Fun():
    yield

class A:
    def Fun(self):
        yield

try:
    print(Fun.__name__)
    print(A.Fun.__name__)
    print(A().Fun.__name__)
except AttributeError:
    print('SKIP')
    raise SystemExit
