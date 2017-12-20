# test compile builtin

def have_compile():
    try:
        compile
        return True
    except NameError:
        return False

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
    exec(compile('print(1 + 1)', 'file', 'single'))
    print(eval(compile('1 + 1', 'file', 'eval')))

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

if have_compile():
    test()
else:
    print("SKIP")
    raise SystemExit
