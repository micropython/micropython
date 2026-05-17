# test overriding builtins

import builtins

# override generic builtin
try:
    builtins.abs = lambda x: x + 1
except AttributeError:
    print("SKIP")
    raise SystemExit

print(abs(1))

# __build_class__ is handled in a special way
orig_build_class = __build_class__
builtins.__build_class__ = lambda x, y: ('class', y)
class A:
    pass
print(A)
builtins.__build_class__ = orig_build_class

# __import__ is handled in a special way
def custom_import(name, globals, locals, fromlist, level):
    print('import', name, fromlist, level)
    class M:
        a = 1
        b = 2
    return M
builtins.__import__ = custom_import
__import__('A', None, None, None, 0)
import a
import a.b
from a import a
from a.b import a, b
from .a import a
from ..a import a, b
