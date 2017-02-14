# test overriding builtins

import builtins

# override generic builtin
try:
    builtins.abs = lambda x: x + 1
except AttributeError:
    import sys
    print("SKIP")
    sys.exit()

print(abs(1))

# __build_class__ is handled in a special way
builtins.__build_class__ = lambda x, y: ('class', y)
class A:
    pass
print(A)
