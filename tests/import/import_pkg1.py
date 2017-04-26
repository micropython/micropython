import pkg.mod

print(pkg.__name__)
print(pkg.mod.__name__)
print(pkg.mod.foo())

# Import 2nd time, must be same module objects
pkg_ = __import__("pkg.mod")
print(pkg_ is not pkg.mod)
print(pkg_ is pkg)
print(pkg_.mod is pkg.mod)

# import using "as"
import pkg.mod as mm
print(mm is pkg.mod)
print(mm.foo())
