# test compile builtin

def have_compile():
    try:
        compile
        return True
    except NameError:
        return False

# global variable for compiled code to access
x = 1

def test():
    c = compile("print(x)", "file", "exec")

    try:
        exec(c)
    except NameError:
        print("NameError")

    exec(c)

    exec(c, {"x":2})
    exec(c, {}, {"x":3})

if have_compile():
    test()
else:
    print("SKIP")
