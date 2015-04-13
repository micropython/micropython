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
