# test attributes of builtin range type

try:
    range(0).start
except AttributeError:
    import sys
    print("SKIP")
    sys.exit()

# attrs
print(range(1, 2, 3).start)
print(range(1, 2, 3).stop)
print(range(1, 2, 3).step)

# bad attr (can't store)
try:
    range(4).start = 0
except AttributeError:
    print('AttributeError')
