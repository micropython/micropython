# tests that import only sets subpackage attribute on first import

import pkg9

pkg9.mod1()
pkg9.mod2()

import pkg9.mod1

pkg9.mod1()
pkg9.mod2()

import pkg9.mod2

pkg9.mod1()
print(pkg9.mod2.__name__, type(pkg9.mod2))
