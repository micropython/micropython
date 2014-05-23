# object.__new__(cls) is the only way in Python to allocate empty
# (non-initialized) instance of class.
# See e.g. http://infohost.nmt.edu/tcc/help/pubs/python/web/new-new-method.html
# TODO: Find reference in CPython docs

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
