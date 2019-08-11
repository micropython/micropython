# Tests against https://www.python.org/dev/peps/pep-0498/

import sys

print(f'no interpolation')
print(f"no interpolation")

# Quoth the PEP:
# Backslashes may not appear anywhere within expressions. Comments, using the
# '#' character, are not allowed inside an expression
#
# CPython (3.7.4 on Linux) raises a SyntaxError here:
# >>> f'{#}'
#   File "<stdin>", line 1
# SyntaxError: f-string expression part cannot include '#'
# >>> f'{\}'
#   File "<stdin>", line 1
# SyntaxError: f-string expression part cannot include a backslash
# >>> f'{\\}'
#   File "<stdin>", line 1
# SyntaxError: f-string expression part cannot include a backslash
# >>> f'{\#}'
#   File "<stdin>", line 1
# SyntaxError: f-string expression part cannot include a backslash

# Backslashes and comments allowed outside expression
print(f"\\")
print(f'#')

## But not inside
try:
    eval("f'{\}'")
except SyntaxError:
    print('SyntaxError')
else:
    print('f-string with backslash in expression did not raise SyntaxError')

try:
    eval("f'{#}'")
except SyntaxError:
    print('SyntaxError')
else:
    print('f-string with \'#\' in expression did not raise SyntaxError')

# Quoth the PEP:
# While scanning the string for expressions, any doubled braces '{{' or '}}'
# inside literal portions of an f-string are replaced by the corresponding
# single brace.  Doubled literal opening braces do not signify the start of an
# expression. A single closing curly brace '}' in the literal portion of a
# string is an error: literal closing curly braces must be doubled '}}' in
# order to represent a single closing brace.
#
# CPython (3.7.4 on Linux) raises a SyntaxError for the last case:
# >>> f'{{}'
#   File "<stdin>", line 1
# SyntaxError: f-string: single '}' is not allowed

print(f'{{}}')

try:
    eval("f'{{}'")
except (ValueError, SyntaxError):
    # MicroPython incorrectly raises ValueError here.
    print('SyntaxError')
else:
    print('Expected ValueError for invalid f-string literal bracing')

x = 1
print(f'{x}')

# Quoth the PEP:
# The expressions that are extracted from the string are evaluated in the
# context where the f-string appeared. This means the expression has full
# access to local and global variables. Any valid Python expression can be
# used, including function and method calls.  Because the f-strings are
# evaluated where the string appears in the source code, there is no additional
# expressiveness available with f-strings. There are also no additional
# security concerns: you could have also just written the same expression, not
# inside of an f-string:

def foo():
    return 20

print(f'result={foo()}', 'result=20')
print(f'result={foo()}', 'result={}'.format(foo()))
print(f'result={foo()}', 'result={result}'.format(result=foo()))

# Other tests
print(f'{{{4*10}}}', '{40}')
