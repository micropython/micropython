# Test weakref.ref() functionality requiring gc.collect().

try:
    import weakref
except ImportError:
    print("SKIP")
    raise SystemExit

# gc module must be available if weakref is.
import gc

def collect_tryveryhard():
    gc.collect()
    try:
        raise Exception  # nlr.ret_val seems to be a major culprit for spurious reachability
    except Exception:
        pass
    gc.collect()

# Cannot reference non-heap objects.
for value in (None, False, True, Ellipsis, 0, "", ()):
    try:
        weakref.ref(value)
    except TypeError:
        print(value, "TypeError")


class A:
    def __str__(self):
        return "<A object>"

class Callback:
    all = []
    def __init__(self, obj=None):
        self.obj = obj
        self.all.append(self)
    def __call__(self, r):
        self.obj = r()
        return self
    def __repr__(self):
        return "callback({})".format(self.obj)

def test():
    print("test basic use of ref() with only one argument")
    a = A()
    r = weakref.ref(a)
    print(r())
    a = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    print(r())

    print("test use of ref() with a callback")
    a = A()
    r = weakref.ref(a, Callback())
    print(r())
    a = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    print(r())

    print("test when weakref gets collected before the object it refs")
    a = A()
    r = weakref.ref(a, Callback())
    print(r())
    r = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    a = None

    print("test a double reference")
    a = A()
    r1 = weakref.ref(a, Callback())
    r2 = weakref.ref(a, Callback())
    print(r1(), r2())
    a = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    print(r1(), r2())

    
    for call in Callback.all:
        print(call)



test()
