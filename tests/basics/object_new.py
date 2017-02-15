# object.__new__(cls) is the only way in Python to allocate empty
# (non-initialized) instance of class.
# See e.g. http://infohost.nmt.edu/tcc/help/pubs/python/web/new-new-method.html
# TODO: Find reference in CPython docs
try:
    # If we don't expose object.__new__ (small ports), there's
    # nothing to test.
    object.__new__
except AttributeError:
    import sys
    print("SKIP")
    sys.exit()

class Foo:

    def __init__(self):
        print("in __init__")
        self.attr = "something"


o = object.__new__(Foo)
#print(o)
print(hasattr(o, "attr"))
print(isinstance(o, Foo))
o.__init__()
#print(dir(o))
print(hasattr(o, "attr"))
print(o.attr)

# should only be able to call __new__ on user types
try:
    object.__new__(1)
except TypeError:
    print("TypeError")
try:
    object.__new__(int)
except TypeError:
    print("TypeError")
