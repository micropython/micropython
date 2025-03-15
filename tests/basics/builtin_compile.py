# test compile builtin

try:
    compile
except NameError:
    print("SKIP")
    raise SystemExit

def test():
    global x

    c = compile("print(x)", "file", "exec")

    try:
        exec(c)
    except NameError:
        print("NameError")

    # global variable for compiled code to access
    x = 1

    exec(c)

    exec(c, {"x":2})
    exec(c, {}, {"x":3})

    # single/eval mode
    exec(compile("if 1: 10 + 1\n", "file", "single"))
    exec(compile("print(10 + 2)", "file", "single"))
    print(eval(compile("10 + 3", "file", "eval")))

    # test accessing a function's globals from within a compile
    exec(compile("def func():pass\nprint('x', func.__globals__['x'])", "file", "exec"))

    # bad mode
    try:
        compile('1', 'file', '')
    except ValueError:
        print("ValueError")

    # exception within compiled code
    try:
        exec(compile('noexist', 'file', 'exec'))
    except NameError:
        print("NameError")
    print(x) # check 'x' still exists as a global

    # hashing a compiled function object
    print(type(hash(compile("", "", "exec"))))

test()
