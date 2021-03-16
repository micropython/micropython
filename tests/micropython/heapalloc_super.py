# test super() operations which don't require allocation
import micropython

# Check for stackless build, which can't call functions without
# allocating a frame on heap.
try:

    def stackless():
        pass

    micropython.heap_lock()
    stackless()
    micropython.heap_unlock()
except RuntimeError:
    print("SKIP")
    raise SystemExit


class A:
    def foo(self):
        print("A foo")
        return 42


class B(A):
    def foo(self):
        print("B foo")
        print(super().foo())


b = B()

micropython.heap_lock()
b.foo()
micropython.heap_unlock()
