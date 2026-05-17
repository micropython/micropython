# This tests relative imports as used in pkg3
import pkg3
import pkg3.mod1
import pkg3.subpkg1.mod1

pkg3.subpkg1.mod1.foo()
