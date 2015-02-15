# builtin type that is not iterable
try:
    for i in 1:
        pass
except TypeError:
    print('TypeError')
