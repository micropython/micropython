# test that __getattr__ and instance members don't override builtins
class C:
    def __init__(self):
        self.__add__ = lambda: print('member __add__')
    def __add__(self, x):
        print('__add__')
    def __getattr__(self, attr):
        print('__getattr__', attr)
        return None

c = C()
c.add # should call __getattr__
c.__add__() # should load __add__ instance directly
c + 1 # should call __add__ method directly
