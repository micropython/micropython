# test exception matching against a tuple

try:
    fail
except (Exception,):
    print('except 1')

try:
    fail
except (Exception, Exception):
    print('except 2')

try:
    fail
except (TypeError, NameError):
    print('except 3')

try:
    fail
except (TypeError, ValueError, Exception):
    print('except 4')
