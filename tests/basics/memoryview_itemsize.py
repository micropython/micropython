try:
    memoryview(b'a').itemsize
except:
    print("SKIP")
    raise SystemExit
try:
    from uarray import array
except ImportError:
    try:
        from array import array
    except ImportError:
        print("SKIP")
        raise SystemExit

for code in ['b', 'h', 'i', 'l', 'q', 'f', 'd']:
    print(memoryview(array(code)).itemsize)

# shouldn't be able to store to the itemsize attribute
try:
    memoryview(b'a').itemsize = 1
except AttributeError:
    print('AttributeError')
