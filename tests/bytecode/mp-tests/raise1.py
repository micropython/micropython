def f():
    raise
def g():
    raise 1
def h():
    raise 1 from 2
def i():
    try:
        f()
    except:
        raise
