from ... import mod1
from ...mod2 import bar

print(mod1.foo)
print(bar)

# attempted relative import beyond top-level package
try:
    from .... import mod1
except ValueError:
    print("ValueError")
