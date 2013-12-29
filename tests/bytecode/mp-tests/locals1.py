# to test the order of locals and arguments (LOAD_FAST, STORE_FAST)

def f1():
    b = 1
    a = 2
    return a + b

def f2(b):
    a = 2
    return a + b

def f3():
    def f3f():
        return True
    a = 1
    return f3f(a)

def f4():
    x = 1
    def f3f():
        return True
    return f3f(x)
