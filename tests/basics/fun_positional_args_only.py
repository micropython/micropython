# positional_args_only tests

# Tests sourced from 
# https://github.com/python/cpython/blob/main/Lib/test/test_positional_only_arg.py

# Not all tests were ported due to incompatibilities with 
# MicroPython, such as required modules like pickle and dis
# not being implemented.

def tryFunctionFail(s):
    try:
        eval(s)
        print("FAILED: " + repr(s) + " did not throw syntax error.")
    except Exception as e:
        print("PASSED: " + repr(s) + " threw an error.")

def tryFunctionPass(s):
    try:
        eval(s)
        print("PASSED: " + repr(s) + " did not throw syntax error.")
    except Exception as e:
        print("FAILED: " + repr(s) + " incorrectly threw syntax error.")

##################################################
# test_invalid_syntax_errors 
# test_invalid_syntax_errors_async
invalid_tests = [
    "def f(a, b = 5, /, c): pass",
    "def f(a = 5, b, /, c): pass",
    "def f(a = 5, b=1, /, c, *, d=2): pass",
    "def f(a = 5, b, /): pass",
    "def f(*args, /): pass",
    "def f(*args, a, /): pass",
    "def f(**kwargs, /): pass",
    "def f(/, a = 1): pass",
    "def f(/, a): pass",
    "def f(/): pass",
    "def f(*, a, /): pass",
    "def f(*, /, a): pass",
    "def f(a, /, a): pass",
    "def f(a, /, *, a): pass",
    "def f(a, b/2, c): pass",
    "def f(a, /, c, /): pass",
    "def f(a, /, c, /, d): pass",
    "def f(a, /, c, /, d, *, e): pass",
    "def f(a, *, c, /, d, e): pass",
    "async def f(a, b = 5, /, c): pass",
    "async def f(a = 5, b, /, c): pass",
    "async def f(a = 5, b=1, /, c, d=2): pass",
    "async def f(a = 5, b, /): pass",
    "async def f(*args, /): pass",
    "async def f(*args, a, /): pass",
    "async def f(**kwargs, /): pass",
    "async def f(/, a = 1): pass",
    "async def f(/, a): pass",
    "async def f(/): pass",
    "async def f(*, a, /): pass",
    "async def f(*, /, a): pass",
    "async def f(a, /, a): pass",
    "async def f(a, /, *, a): pass",
    "async def f(a, b/2, c): pass",
    "async def f(a, /, c, /): pass",
    "async def f(a, /, c, /, d): pass",
    "async def f(a, /, c, /, d, *, e): pass",
    "async def f(a, *, c, /, d, e): pass"
]
for s in invalid_tests:
    tryFunctionFail(s)

##################################################
# test_optional_positional_only_args
print("\ntest_optional_positional_only_args")
def assertEqual(f, res):
    assert(f == res)
    print("PASSED: Assertion check")

def f(a, b=10, /, c=100):
    return a + b + c

assertEqual(f(1, 2, 3), 6)
assertEqual(f(1, 2, c=3), 6)
tryFunctionFail("f(1, b=2, c=3)")

assertEqual(f(1, 2), 103)
tryFunctionFail("f(1, b=2)")
assertEqual(f(1, c=2), 13)

def f(a=1, b=10, /, c=100):
    return a + b + c

assertEqual(f(1, 2, 3), 6)
assertEqual(f(1, 2, c=3), 6)
tryFunctionFail("f(1, b=2, c=3)")

assertEqual(f(1, 2), 103)
tryFunctionFail("f(1, b=2)")
assertEqual(f(1, c=2), 13)

##################################################
# test_pos_only_call_via_unpacking
print("\ntest_pos_only_call_via_unpacking")
def f(a, b, /):
    return a + b
assertEqual(f(*[1, 2]), 3)

##################################################
# test_use_positional_as_keyword
print("\ntest_use_positional_as_keyword")
def f(a, /):
    pass
expected = r"f() got some positional-only arguments passed as keyword arguments: 'a'"
tryFunctionFail("f(a=1)")

def f(a, /, b):
    pass
expected = r"f() got some positional-only arguments passed as keyword arguments: 'a'"
tryFunctionFail("f(a=1, b=2)")

def f(a, b, /):
    pass
expected = r"f() got some positional-only arguments passed as keyword arguments: 'a, b'"
tryFunctionFail("f(a=1, b=2)")

##################################################
# test_positional_only_and_arg_invalid_calls
print("\ntest_positional_only_and_arg_invalid_calls")

def f(a, b, /, c):
    pass
# r"f() missing 1 required positional argument: 'c'"
tryFunctionFail("f(1, 2)")
# r"f() missing 2 required positional arguments: 'b' and 'c'"
tryFunctionFail("f(1)")
# r"f() missing 3 required positional arguments: 'a', 'b', and 'c'"
tryFunctionFail("f()")
# r"f() takes 3 positional arguments but 4 were given"
tryFunctionFail("f(1, 2, 3, 4)")

##################################################
# test_positional_only_and_optional_arg_invalid_calls
print("\ntest_positional_only_and_optional_arg_invalid_calls")
def f(a, b, /, c=3):
    pass
tryFunctionPass("f(1, 2)")
# "f() missing 1 required positional argument: 'b'"
tryFunctionFail("f(1)")
# "f() missing 2 required positional arguments: 'a' and 'b'"
tryFunctionFail("f()")
# "f() takes from 2 to 3 positional arguments but 4 were given"
tryFunctionFail("f(1, 2, 3, 4)")

##################################################
# test_positional_only_and_kwonlyargs_invalid_calls
def f(a, b, /, c, *, d, e):
    pass
tryFunctionPass("f(1, 2, 3, d=1, e=2)")
# r"missing 1 required keyword-only argument: 'd'"
tryFunctionFail("f(1, 2, 3, e=2)")
# r"missing 2 required keyword-only arguments: 'd' and 'e'"
tryFunctionFail("f(1, 2, 3)")
# r"f() missing 1 required positional argument: 'c'"
tryFunctionFail("f(1, 2)")
# r"f() missing 2 required positional arguments: 'b' and 'c'"
tryFunctionFail("f(1)")
# r" missing 3 required positional arguments: 'a', 'b', and 'c'"
tryFunctionFail("f()")
# r"f() takes 3 positional arguments but 6 positional arguments \(and 2 keyword-only arguments\) were given"
tryFunctionFail("f(1, 2, 3, 4, 5, 6, d=7, e=8)")
# r"f() got an unexpected keyword argument 'f'"
tryFunctionFail("f(1, 2, 3, d=1, e=4, f=56)")

##################################################
# test_positional_only_invalid_calls
def f(a, b, /):
    pass
tryFunctionPass("f(1, 2)")
# f() missing 1 required positional argument: 'b'"
tryFunctionFail("f(1)")
# f() missing 2 required positional arguments: 'a' and 'b'"
tryFunctionFail("f()")
# f() takes 2 positional arguments but 3 were given"
tryFunctionFail("f(1, 2, 3)")

##################################################
# test_positional_only_with_optional_invalid_calls
def f(a, b=2, /):
    pass
tryFunctionPass("f(1)")
# "f() missing 1 required positional argument: 'a'"
tryFunctionFail("f()")
# "f() takes from 1 to 2 positional arguments but 3 were given"
tryFunctionFail("f(1, 2, 3)")

##################################################
# test_no_standard_args_usage
print("\ntest_no_standard_args_usage")
def f(a, b, /, *, c):
    pass

tryFunctionPass("f(1, 2, c=3)")
# TypeError
tryFunctionFail("f(1, b=2, c=3)")

##################################################
# test_lambdas
print("\ntest_lambdas")
try:
    x = lambda a, /, b: a + b
    assertEqual(x(1,2), 3)
    assertEqual(x(1,b=2), 3)

    x = lambda a, /, b=2: a + b
    assertEqual(x(1), 3)

    x = lambda a, b, /: a + b
    assertEqual(x(1, 2), 3)

    x = lambda a, b, /, : a + b
    assertEqual(x(1, 2), 3)
except:
    print("FAILED: test_lambdas not working")

##################################################
# test_invalid_syntax_lambda
try:
    tryFunctionFail("lambda a, b = 5, /, c: None")
    tryFunctionFail("lambda a = 5, b, /, c: None")
    tryFunctionFail("lambda a = 5, b, /: None")
    tryFunctionFail("lambda *args, /: None")
    tryFunctionFail("lambda *args, a, /: None")
    tryFunctionFail("lambda **kwargs, /: None")
    tryFunctionFail("lambda /, a = 1: None")
    tryFunctionFail("lambda /, a: None")
    tryFunctionFail("lambda /: None")
    tryFunctionFail("lambda *, a, /: None")
    tryFunctionFail("lambda *, /, a: None")
    tryFunctionFail("lambda a, /, a: None")
    tryFunctionFail("lambda a, /, *, a: None")
    tryFunctionFail("lambda a, /, b, /: None")
    tryFunctionFail("lambda a, /, b, /, c: None")
    tryFunctionFail("lambda a, /, b, /, c, *, d: None")
    tryFunctionFail("lambda a, *, b, /, c: None")
except:
    print("FAILED: test_invalid_syntax_lambda not working")

##################################################
# test_posonly_methods
print("\ntest_posonly_methods")
class Example:
    def f(self, a, b, /):
        return a, b

assertEqual(Example().f(1, 2), (1, 2))
assertEqual(Example.f(Example(), 1, 2), (1, 2))
tryFunctionFail("Example.f(1,2)")
# f() got some positional-only arguments passed as keyword arguments: 'b'
tryFunctionFail("Example().f(1, b=2)")

##################################################
# test_module_function
print("\ntest_module_function")
def f(a, b, /):
    pass
tryFunctionFail("f()")

##################################################
# test_closures
print("\ntest_closures")
def f(x,y):
    def g(x2,/,y2):
        return x + y + x2 + y2
    return g

assertEqual(f(1,2)(3,4), 10)
tryFunctionFail("f(1,2)(3)")
tryFunctionFail("f(1,2)(3,4,5)")

def f(x,/,y):
    def g(x2,y2):
        return x + y + x2 + y2
    return g
assertEqual(f(1,2)(3,4), 10)

def f(x,/,y):
    def g(x2,/,y2):
        return x + y + x2 + y2
    return g
assertEqual(f(1,2)(3,4), 10)
tryFunctionFail("f(1,2)(3)")
tryFunctionFail("f(1,2)(3,4,5)")


##################################################
# test_same_keyword_as_positional_with_kwargs
print("\ntest_same_keyword_as_positional_with_kwargs")
def f(something,/,**kwargs):
    return (something, kwargs)

assertEqual(f(42, something=42), (42, {'something': 42}))
tryFunctionFail("f(something=42)")
assertEqual(f(42), (42, {}))


##################################################
# test_mangling
print("\ntest_mangling")
class X:
    def f(self, __a=42, /):
        return __a

    def f2(self, __a=42, /, __b=43):
        return (__a, __b)

    def f3(self, __a=42, /, __b=43, *, __c=44):
        return (__a, __b, __c)

assertEqual(X().f(), 42)
assertEqual(X().f2(), (42, 43))
assertEqual(X().f3(), (42, 43, 44))

##################################################
# test_too_many_arguments
# Ignored due to bug with compile function in micropython-minimal
# print("\ntest_too_many_arguments")
# fundef = "def f(" + ', '.join('i' + str(i) for i in range(300)) + ", /):\n  pass\n"
# compile(fundef, "<test>", "single")
# print("PASSED: > 255 arg check")

##################################################
# test_async
print("\ntest_async")
async def f(a=1, /, b=2):
    return a, b

tryFunctionFail("f(a=1, b=2)")

def _check_call(*args, **kwargs):
    try:
        coro = f(*args, **kwargs)
        coro.send(None)
    except StopIteration as e:
        result = e.value
    assertEqual(result, (1, 2))

_check_call(1, 2)
_check_call(1, b=2)
_check_call(1)
_check_call()


##################################################
# test_generator
print("\ntest_generator")
def f(a=1, /, b=2):
    yield a, b

tryFunctionFail("f(a=1, b=2)")

gen = f(1, 2)
assertEqual(next(gen), (1, 2))
gen = f(1, b=2)
assertEqual(next(gen), (1, 2))
gen = f(1)
assertEqual(next(gen), (1, 2))
gen = f()
assertEqual(next(gen), (1, 2))


##################################################
# test_super
print("\ntest_super")
sentinel = object()

class A:
    def method(self):
        return sentinel

class C(A):
    def method(self, /):
        return super().method()

assertEqual(C().method(), sentinel)