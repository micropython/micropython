# Test interning qstrs that go over the limit of the maximum qstr length
# (which is 255 bytes for the default configuration)


def make_id(n, base="a"):
    return "".join(chr(ord(base) + i % 26) for i in range(n))


# identifiers in parser
for l in range(254, 259):
    g = {}
    var = make_id(l)
    try:
        exec(var + "=1", g)
    except RuntimeError as er:
        print("RuntimeError", er, l)
        continue
    print(var in g)


# calling a function with kwarg
def f(**k):
    print(k)


for l in range(254, 259):
    try:
        exec("f({}=1)".format(make_id(l)))
    except RuntimeError as er:
        print("RuntimeError", er, l)

# type construction
for l in range(254, 259):
    id = make_id(l)
    try:
        print(type(id, (), {}))
    except RuntimeError as er:
        print("RuntimeError", er, l)


# hasattr, setattr, getattr
class A:
    pass


for l in range(254, 259):
    id = make_id(l)
    a = A()
    try:
        setattr(a, id, 123)
    except RuntimeError as er:
        print("RuntimeError", er, l)
    try:
        print(hasattr(a, id), getattr(a, id))
    except RuntimeError as er:
        print("RuntimeError", er, l)

# format with keys
for l in range(254, 259):
    id = make_id(l)
    try:
        print(("{" + id + "}").format(**{id: l}))
    except RuntimeError as er:
        print("RuntimeError", er, l)

# import module
# (different OS's have different results so only run those that are consistent)
for l in (100, 101, 256, 257, 258):
    try:
        __import__(make_id(l))
    except ImportError:
        print("ok", l)
    except RuntimeError as er:
        print("RuntimeError", er, l)

# import package
for l in (100, 101, 102, 128, 129):
    try:
        exec("import " + make_id(l) + "." + make_id(l, "A"))
    except ImportError:
        print("ok", l)
    except RuntimeError as er:
        print("RuntimeError", er, l)
