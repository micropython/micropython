from pkg import mod

print(mod.foo())

import pkg.mod

print(mod is pkg.mod)
