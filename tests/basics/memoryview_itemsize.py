try:
    memoryview(b'a').itemsize
    from array import array
except:
    print("SKIP")
    raise SystemExit

for code in ['b', 'h', 'i', 'l', 'q', 'f', 'd']:
    print(memoryview(array(code)).itemsize)

# shouldn't be able to store to the itemsize attribute
try:
    memoryview(b'a').itemsize = 1
except AttributeError:
    print('AttributeError')
