class MyMeta(type):
    def __init__(cls, name, bases, dct):
        print('meta', '__init__')
        super().__init__(name, bases, dct)
        print(cls.__name__)
        cls.extra = 'my_meta_was_here'


class A(metaclass=MyMeta):
    def __init__(self):
        print('A', '__init__')
        assert type(type(self)) is MyMeta
        assert type(self).extra == 'my_meta_was_here'

A()
