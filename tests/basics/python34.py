# tests that differ when running under Python 3.4 vs 3.5/3.6/3.7

try:
    exec
except NameError:
    print("SKIP")
    raise SystemExit

# from basics/fun_kwvarargs.py
# test evaluation order of arguments (in 3.4 it's backwards, 3.5 it's fixed)
def f4(*vargs, **kwargs):
    print(vargs, kwargs)
def print_ret(x):
    print(x)
    return x
f4(*print_ret(['a', 'b']), kw_arg=print_ret(None))

# test evaluation order of dictionary key/value pair (in 3.4 it's backwards)
{print_ret(1):print_ret(2)}

# from basics/syntaxerror.py
def test_syntax(code):
    try:
        exec(code)
    except SyntaxError:
        print("SyntaxError")
test_syntax("f(*a, *b)") # can't have multiple * (in 3.5 we can)
test_syntax("f(**a, **b)") # can't have multiple ** (in 3.5 we can)
test_syntax("f(*a, b)") # can't have positional after *
test_syntax("f(**a, b)") # can't have positional after **
test_syntax("() = []") # can't assign to empty tuple (in 3.6 we can)
test_syntax("del ()") # can't delete empty tuple (in 3.6 we can)

# from basics/sys1.py
# uPy prints version 3.4
import usys
print(usys.version[:3])
print(usys.version_info[0], usys.version_info[1])

# from basics/exception1.py
# in 3.7 no comma is printed if there is only 1 arg (in 3.4-3.6 one is printed)
print(repr(IndexError("foo")))
