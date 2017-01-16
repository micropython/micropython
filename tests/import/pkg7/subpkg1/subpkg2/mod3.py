from ... import mod1
from ...mod2 import bar
print(mod1.foo)
print(bar)

# when attempting relative import beyond top-level package uPy raises ImportError
# whereas CPython raises a ValueError
try:
    from .... import mod1
except ValueError:
    print('ValueError')
