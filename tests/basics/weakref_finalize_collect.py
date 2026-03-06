# Test weakref.finalize() functionality requiring gc.collect().

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


class A:
    def __str__(self):
        return "<A object>"


class Callback:
    all = []
    def __init__(self):
        self.args = None
        self.kwargs = None
        self.all.append(self)
    def __call__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
        return self
    def __repr__(self):
        return "callback()(*{}, **{})".format(self.args, self.kwargs)

def test():
    print("test basic use of finalize() with a simple callback")
    a = A()
    f = weakref.finalize(a, Callback())
    a = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    print("alive", f.alive)
    print("peek", f.peek())
    print("detach", f.detach())
    print("call", f())

    print("test that a callback is passed the correct values")
    a = A()
    f = weakref.finalize(a, Callback(), 1, 2, kwarg=3)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    print("alive", f.alive)
    print("peek", f.peek())
    print("detach", f.detach())
    print("call", f())

    print("test that calling the finalizer cancels the finalizer")
    a = A()
    f = weakref.finalize(a, Callback())
    print(f())
    print(a)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()

    print("test that calling detach cancels the finalizer")
    a = A()
    f = weakref.finalize(a, Callback())
    print(len(f.detach()))
    print(a)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()

    print("test that finalize does not get collected before its ref does")
    a = A()
    weakref.finalize(a, Callback())
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    print("free a")
    a = None
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()
    clean_the_stack = [0, 0, 0, 0]
    collect_tryveryhard()

    for call in Callback.all:
        print(call)


test()
