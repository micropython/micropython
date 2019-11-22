# Test interning qstrs that go over the limit of the maximum qstr length
# (which is 255 bytes for the default configuration)

def make_id(n, base='a'):
    return ''.join(chr(ord(base) + i % 26) for i in range(n))

# identifiers in parser
for l in range(254, 259):
    g = {}
    var = make_id(l)
    try:
        exec(var + '=1', g)
    except RuntimeError:
        print('RuntimeError', l)
        continue
    print(var in g)

# calling a function with kwarg
def f(**k):
    print(k)
for l in range(254, 259):
    try:
        exec('f({}=1)'.format(make_id(l)))
    except RuntimeError:
        print('RuntimeError', l)

# type construction
for l in range(254, 259):
    id = make_id(l)
    try:
        print(type(id, (), {}).__name__)
    except RuntimeError:
        print('RuntimeError', l)

# hasattr, setattr, getattr
class A:
    pass
for l in range(254, 259):
    id = make_id(l)
    a = A()
    try:
        setattr(a, id, 123)
    except RuntimeError:
        print('RuntimeError', l)
    try:
        print(hasattr(a, id), getattr(a, id))
    except RuntimeError:
        print('RuntimeError', l)

# format with keys
for l in range(254, 259):
    id = make_id(l)
    try:
        print(('{' + id + '}').format(**{id: l}))
    except RuntimeError:
        print('RuntimeError', l)

# modulo format with keys
for l in range(254, 259):
    id = make_id(l)
    try:
        print(('%(' + id + ')d') % {id: l})
    except RuntimeError:
        print('RuntimeError', l)

# import module
# (different OS's have different results so only print those that are consistent)
for l in range(150, 259):
    try:
        __import__(make_id(l))
    except ImportError:
        if l < 152:
            print('ok', l)
    except RuntimeError:
        if l > 255:
            print('RuntimeError', l)

# import package
for l in range(125, 130):
    try:
        exec('import ' + make_id(l) + '.' + make_id(l, 'A'))
    except ImportError:
        print('ok', l)
    except RuntimeError:
        print('RuntimeError', l)
