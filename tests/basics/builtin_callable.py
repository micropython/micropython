# test builtin callable

# primitives should not be callable
print(callable(None))
print(callable(1))
print(callable([]))
print(callable("dfsd"))

# modules should not be callabe
try:
    import usys as sys
except ImportError:
    import sys
print(callable(sys))

# builtins should be callable
print(callable(callable))

# lambdas should be callable
print(callable(lambda:None))

# user defined functions should be callable
def f():
    pass
print(callable(f))

# types should be callable, but not instances
class A:
    pass
print(callable(A))
print(callable(A()))

# instances with __call__ method should be callable
class B:
    def __call__(self):
        pass
print(callable(B()))

# this checks internal use of callable when extracting members from an instance
class C:
    def f(self):
        return "A.f"
class D:
    g = C() # g is a value and is not callable
print(callable(D().g))
print(D().g.f())
