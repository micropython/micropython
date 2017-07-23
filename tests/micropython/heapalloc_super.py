# test super() operations which don't require allocation
import micropython

class A:
    def foo(self):
        print('A foo')
        return 42
class B(A):
    def foo(self):
        print('B foo')
        print(super().foo())

b = B()

micropython.heap_lock()
b.foo()
micropython.heap_unlock()
