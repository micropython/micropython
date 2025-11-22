# test SyntaxError with := operator

def test(code):
    try:
        print(eval(code))
    except SyntaxError:
        print('SyntaxError')

def test_exec(code):
    try:
        exec(code)
        print('no SyntaxError')
    except SyntaxError:
        print('SyntaxError')

test("x := 1")
test("((x, y) := 1)")
test("([i := i + 1 for i in range(4)])")
test("([i := -1 for i, j in [(1, 2)]])")
test("([[(i := j) for i in range(2)] for j in range(2)])")

# this is currently allowed in MicroPython, but not in CPython
test("([[(j := i) for i in range(2)] for j in range(2)])")

# walrus expression as positional argument after keyword argument
test_exec("print(end='', (x:=1))")

# additional walrus after keyword tests using compile() for better coverage
def test_compile(code):
    try:
        compile(code, "", "exec")
        print('no SyntaxError')
    except SyntaxError:
        print('SyntaxError')

test_compile("f(x=1, y:=2)")
test_compile("f(a=1, b=2, c:=3)")
