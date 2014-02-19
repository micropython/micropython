from pkg.mod import foo

try:
    pkg
except NameError:
    print("NameError")
try:
    pkg.mod
except NameError:
    print("NameError")
print(foo())

# Import few times, must be same module objects
mod_1 = __import__("pkg.mod", None, None, ("foo",))
mod_2 = __import__("pkg.mod", None, None, ("foo",))
print(mod_1 is mod_2)
print(mod_1.foo is mod_2.foo)
print(foo is mod_1.foo)
