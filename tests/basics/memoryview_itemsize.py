try:
    memoryview(b'a').itemsize
except:
    print("SKIP")
    raise SystemExit
try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

for code in ['b', 'h', 'i', 'q', 'f', 'd']:
    print(memoryview(array(code)).itemsize)

# 'l' varies depending on word size of the machine
print(memoryview(array('l')).itemsize in (4, 8))

# shouldn't be able to store to the itemsize attribute
try:
    memoryview(b'a').itemsize = 1
except AttributeError:
    print('AttributeError')
