# test builtin dir

# dir of locals
print('__name__' in dir())

# dir of module
import sys
print('platform' in dir(sys))

# dir of type
print('append' in dir(list))

class Foo:
    def __init__(self):
        self.x = 1
foo = Foo()
print('__init__' in dir(foo))
print('x' in dir(foo))

