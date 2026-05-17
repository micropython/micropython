# test break within (nested) finally

# basic case with break in finally
def f():
    for _ in range(2):
        print(1)
        try:
            pass
        finally:
            print(2)
            break
            print(3)
        print(4)
    print(5)
f()

# where the finally swallows an exception
def f():
    lst = [1, 2, 3]
    for x in lst:
        print('a', x)
        try:
            raise Exception
        finally:
            print(1)
            break
        print('b', x)
f()

# basic nested finally with break in inner finally
def f():
    for i in range(2):
        print('iter', i)
        try:
            raise TypeError
        finally:
            print(1)
            try:
                raise ValueError
            finally:
                break
print(f())

# similar to above but more nesting
def f():
    for i in range(2):
        try:
            raise ValueError
        finally:
            print(1)
            try:
                raise TypeError
            finally:
                print(2)
                try:
                    pass
                finally:
                    break
print(f())

# lots of nesting
def f():
    for i in range(2):
        try:
            raise ValueError
        finally:
            print(1)
            try:
                raise TypeError
            finally:
                print(2)
                try:
                    raise Exception
                finally:
                    break
print(f())

# basic case combined with try-else
def f(arg):
    for _ in range(2):
        print(1)
        try:
            if arg == 1:
                raise ValueError
            elif arg == 2:
                raise TypeError
        except ValueError:
            print(2)
        else:
            print(3)
        finally:
            print(4)
            break
            print(5)
        print(6)
    print(7)
f(0) # no exception, else should execute
f(1) # exception caught, else should be skipped
f(2) # exception not caught, finally swallows exception, else should be skipped
