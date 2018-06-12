# tests for things that only Python 3.6 supports, needing floats

try:
    eval
except NameError:
    print("SKIP")
    raise SystemExit

# underscores in numeric literals
print(eval('1_000.1_8'))
print('%.2g' % eval('1e1_2'))

# underscore supported by int/float constructors
print(float('1_2_3'))
print(float('1_2_3.4'))
print('%.2g' % float('1e1_3'))
