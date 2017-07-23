# test calling builtin import function

# basic test
__import__('builtins')

# first arg should be a string
try:
    __import__(1)
except TypeError:
    print('TypeError')

# level argument should be non-negative
try:
    __import__('xyz', None, None, None, -1)
except ValueError:
    print('ValueError')
