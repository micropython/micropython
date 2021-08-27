try:
    type('abc', None, None)
except TypeError:
    print(True)
else:
    print(False)

try:
    type('abc', (), None)
except TypeError:
    print(True)
else:
    print(False)

try:
    type('abc', (1,), {})
except TypeError:
    print(True)
else:
    print(False)
