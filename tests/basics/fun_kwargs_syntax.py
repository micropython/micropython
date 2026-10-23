# Test function call keyword argument syntax

def test_syntax(code):
    try:
        eval(code)
    except SyntaxError:
        print("SyntaxError in '{}'".format(code))

def f(a):
    return a

# Should throw syntax errors.
test_syntax("f((a)=2)")
test_syntax("f(a()=2)")
test_syntax("f(a or b=2)")
test_syntax("f(2, (a)=3)")
test_syntax("f((a) = 1, (b) = 5)")
test_syntax("f((a, b) = 1)")
test_syntax("f(f(), (a)=2)")
