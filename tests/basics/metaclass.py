import sys
is_micropython = sys.implementation.name == 'micropython'

class MyMeta(type):
    def __init__(cls, name, bases, dct):
        print('MyMeta', '__init__')
        super().__init__(name, bases, dct)
        cls.extra2 = 'my_meta_was_here'

    def __new__(cls, name, bases, dct):
        print('MyMeta', '__new__')
        if is_micropython:
            return type(name, bases, dct)
        else:
            return type.__new__(cls, name, bases, dct)

class B:
    def __init__(self):
        print('B', '__init__')

class A(metaclass=MyMeta):
    def __init__(self):
        print('A', '__init__')
        print(type(self).extra2 == 'my_meta_was_here')

class C(metaclass=MyMeta):
    def __init__(self):
        print('C', '__init__')

class D(B):
    def __init__(self):
        print('D', '__init__')
        super().__init__()

class E(B):
    def __init__(self):
        print('E', '__init__')
        super().__init__()

for cls in (A, B, C, D, E):
    try:
        qq = cls()
    except Exception as e:
        print(repr(e))

