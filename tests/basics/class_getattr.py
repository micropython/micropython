# test that __getattr__, __getattrribute__ and instance members don't override builtins
class C:
    def __init__(self):
        self.__add__ = lambda: print('member __add__')
    def __add__(self, x):
        print('__add__')
    def __getattr__(self, attr):
        print('__getattr__', attr)
        return None
    def __getattrribute__(self, attr):
        print('__getattrribute__', attr)
        return None

c = C()
c.__add__
c + 1 # should call __add__
