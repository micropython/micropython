# test 'return' within the finally block
# it should swallow the exception

# simple case
def f():
    try:
        raise ValueError()
    finally:
        print('finally')
        return 0
    print('got here')
print(f())

# nested, return in outer
def f():
    try:
        try:
            raise ValueError
        finally:
            print('finally 1')
        print('got here')
    finally:
        print('finally 2')
        return 2
    print('got here')
print(f())

# nested, return in inner
def f():
    try:
        try:
            raise ValueError
        finally:
            print('finally 1')
            return 1
        print('got here')
    finally:
        print('finally 2')
    print('got here')
print(f())

# nested, return in inner and outer
def f():
    try:
        try:
            raise ValueError
        finally:
            print('finally 1')
            return 1
        print('got here')
    finally:
        print('finally 2')
        return 2
    print('got here')
print(f())

# nested with reraise
def f():
    try:
        try:
            raise ValueError
        except:
            raise
        print('got here')
    finally:
        print('finally')
        return 0
    print('got here')
print(f())

# triple nesting with reraise
def f():
    try:
        try:
            try:
                raise ValueError
            except:
                raise
        except:
            raise
    finally:
        print('finally')
        return 0
print(f())

# exception when matching exception
def f():
    try:
        raise ValueError
    except NonExistingError:
        pass
    finally:
        print('finally')
        return 0
print(f())

# raising exception class, not instance
def f():
    try:
        raise ValueError
    finally:
        print('finally')
        return 0
print(f())
