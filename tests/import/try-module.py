# Regression test for #290 - throwing exception in another module led to
# its namespace stick and namespace of current module not coming back.
import import1b

def func1():
    print('func1')

def func2():
    try:
        import1b.throw()
    except ValueError:
        pass
    func1()

func2()
