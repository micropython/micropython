### Test decorator syntax.

###############################################################################
### Test for syntax errors.
###############################################################################
def test_for_syntax_error(code):
    try:
        exec(code)
    except SyntaxError:
        print("SyntaxError")

decs = [
    "x,",
    "x, y",
    "x = y",
    "pass",
    "import sys",
    "@dec"
]

for dec in decs:
    code = "@{}\ndef foo(): pass".format(dec)
    test_for_syntax_error(code)

###############################################################################
### Test for type errors (previously syntax errors before Python 3.9).
###############################################################################
def test_for_type_error(code):
    try:
        exec(code)
    except TypeError:
        print("TypeError")

decs = [
    "[1, 2][-1]",
    "(1, 2)",
    "True",
    "...",
    "None"
]

for dec in decs:
    code = "@{}\ndef foo(): pass".format(dec)
    test_for_type_error(code)

###############################################################################
### Test for name errors. Tests added to make sure the compiler does not
### confuse anything as a built-in decorator.
###############################################################################
def test_for_name_error(code):
    try:
        exec(code)
    except NameError:
        print("NameError")

decs = [
    "some",
    "some.dotted",
    "some.dotted.name"
]

for dec in decs:
    code = "@{}\ndef foo(): pass".format(dec)
    test_for_name_error(code)

###############################################################################
### Test valid decorators.
###############################################################################
def dec(f):
    print('dec')
    return f

def dec_arg(x):
    print(x)
    return lambda f:f

# plain decorator
@dec
def f():
    pass

# decorator with arg
@dec_arg('dec_arg')
def g():
    pass

# decorator of class
@dec
class A:
    pass
