from ... import mod1
from ...mod2 import bar

print(mod1.foo)
print(bar)

# attempted relative import beyond top-level package
# On older versions of CPython (eg 3.8) this is a ValueError, but on
# newer CPython (eg 3.11) and MicroPython it's an ImportError.
try:
    from .... import mod1
except (ImportError, ValueError):
    print("ImportError")
