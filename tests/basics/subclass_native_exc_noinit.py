# Test that exception subclasses work even if super().__init__() is not called.
# This tests the fix for a crash in get_native_exception() when subobj[0]
# contains the sentinel value (native_base_init_wrapper_obj).
#
# The crash originally occurred when an exception without initialized native base
# propagated through generator frames, triggering traceback addition code.

class FooError(RuntimeError):
    def __init__(self, msg=()):
        # Deliberately NOT calling super().__init__()
        pass

class NoFoo(FooError):
    pass

# Test 1: Basic raise/catch
try:
    raise NoFoo()
except NoFoo as e:
    print("caught NoFoo")

# Test 2: Exception propagating through multiple generator frames
# This is the pattern that triggers the crash - traceback gets added
# as exception propagates through each generator frame
def gen1():
    yield 1
    def gen2():
        yield 2
        def gen3():
            yield 3
            raise NoFoo()
        g3 = gen3()
        next(g3)
        yield next(g3)
    g2 = gen2()
    next(g2)
    yield next(g2)

try:
    g = gen1()
    next(g)
    next(g)
except NoFoo:
    print("caught from nested generators")

# Test 3: Exception created via __new__ returning different subclass
class ErrorFactory(RuntimeError):
    def __new__(cls, code):
        if code == 1:
            return super().__new__(NoFoo)
        return super().__new__(cls)
    def __init__(self, code):
        pass  # No super().__init__()

def gen_factory():
    yield 1
    raise ErrorFactory(1)  # Returns NoFoo instance

try:
    g = gen_factory()
    next(g)
    next(g)
except NoFoo:
    print("caught factory-created exception")

print("done")
