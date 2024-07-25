# test calling __set_name__ during class creation
# https://docs.python.org/3/reference/datamodel.html#object.__set_name__

class A:
    def __set_name__(self, owner, name):
        print("owner", owner.__name__)
        print("name", name)

class B:
    a = A()

# skip failing on minimal port without descriptor support
b = B()
try:
    b.__class__
except AttributeError:
    print("SKIP")
    raise SystemExit
