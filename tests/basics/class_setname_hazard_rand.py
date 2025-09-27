# Test to make sure there's no sequence hazard even when a __set_name__ implementation
# mutates and reorders the namespace of its owner class.
# VERY hard bug to prove out except via a stochastic test.


try:
    from random import choice
    import re
except ImportError:
    print("SKIP")
    raise SystemExit


def skip_if_no_descriptors():
    class Descriptor:
        def __get__(self, obj, cls):
            return

    class TestClass:
        Forward = Descriptor()

    a = TestClass()
    try:
        a.__class__
    except AttributeError:
        # Target doesn't support __class__.
        print("SKIP")
        raise SystemExit

    b = a.Forward
    if "Descriptor" in repr(b.__class__):
        # Target doesn't support descriptors.
        print("SKIP")
        raise SystemExit


skip_if_no_descriptors()

letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

# Would be r"[A-Z]{5}", but not all ports support the {n} quantifier.
junk_re = re.compile(r"[A-Z][A-Z][A-Z][A-Z][A-Z]")


def junk_fill(obj, n=10):  # Add randomly-generated attributes to an object.
    for i in range(n):
        name = "".join(choice(letters) for j in range(5))
        setattr(obj, name, object())


def junk_clear(obj):  # Remove attributes added by junk_fill.
    to_del = [name for name in dir(obj) if junk_re.match(name)]
    for name in to_del:
        delattr(obj, name)


def junk_sequencer():
    global runs
    try:
        while True:
            owner, name = yield
            runs[name] = runs.get(name, 0) + 1
            junk_fill(owner)
    finally:
        junk_clear(owner)


class JunkMaker:
    def __set_name__(self, owner, name):
        global seq
        seq.send((owner, name))


runs = {}
seq = junk_sequencer()
next(seq)


class Main:
    a = JunkMaker()
    b = JunkMaker()
    c = JunkMaker()
    d = JunkMaker()
    e = JunkMaker()
    f = JunkMaker()
    g = JunkMaker()
    h = JunkMaker()
    i = JunkMaker()
    j = JunkMaker()
    k = JunkMaker()
    l = JunkMaker()
    m = JunkMaker()
    n = JunkMaker()
    o = JunkMaker()
    p = JunkMaker()
    q = JunkMaker()
    r = JunkMaker()
    s = JunkMaker()
    t = JunkMaker()
    u = JunkMaker()
    v = JunkMaker()
    w = JunkMaker()
    x = JunkMaker()
    y = JunkMaker()
    z = JunkMaker()


seq.close()

for k in letters.lower():
    print(k, runs.get(k, 0))
