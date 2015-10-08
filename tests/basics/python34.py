# tests that differ when running under Python 3.4 vs 3.5

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
# can't have multiple * or ** (in 3.5 we can)
def test_syntax(code):
    try:
        exec(code)
    except SyntaxError:
        print("SyntaxError")
test_syntax("f(*a, *b)")
test_syntax("f(**a, **b)")

# from basics/sys1.py
# uPy prints version 3.4
import sys
print(sys.version[:3])
print(sys.version_info[0], sys.version_info[1])
