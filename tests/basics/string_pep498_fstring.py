# Tests against https://www.python.org/dev/peps/pep-0498/

assert f'no interpolation' == 'no interpolation'
assert f"no interpolation" == 'no interpolation'

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
assert f"\\" == "\\"
assert f'#' == '#'

## But not inside
try:
    eval("f'{\}'")
except SyntaxError:
    pass
else:
    raise AssertionError('f-string with backslash in expression did not raise SyntaxError')

try:
    eval("f'{#}'")
except SyntaxError:
    pass
else:
    raise AssertionError('f-string with \'#\' in expression did not raise SyntaxError')

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

assert f'{{}}' == '{}'

try:
    eval("f'{{}'")
except ValueError:
    pass
else:
    raise RuntimeError('Expected ValueError for invalid f-string literal bracing')

x = 1
assert f'{x}' == '1'

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

assert f'result={foo()}' == 'result=20'
assert f'result={foo()}' == 'result={}'.format(foo())
assert f'result={foo()}' == 'result={result}'.format(result=foo())


# Quoth the PEP:
# Adjacent f-strings and regular strings are concatenated. Regular strings are
# concatenated at compile time, and f-strings are concatenated at run time. For
# example, the expression:
#
# >>> x = 10
# >>> y = 'hi'
# >>> 'a' 'b' f'{x}' '{c}' f'str<{y:^4}>' 'd' 'e'
#
# yields the value: 'ab10{c}str< hi >de'
#
# Because strings are concatenated at lexer time rather than parser time in
# MicroPython for mostly RAM efficiency reasons (see
# https://github.com/micropython/micropython/commit/534b7c368dc2af7720f3aaed0c936ef46d773957),
# and because f-strings here are implemented as a syntax translation
# (f'{something}' => '{}'.format(something)), this particular functionality is unimplemented,
# and in the above example, the '{c}' portion will trigger a KeyError on String.format()

x = 10
y = 'hi'
assert (f'h' f'i') == 'hi'
#assert (f'h' 'i') == 'hi'
#assert ('h' f'i') == 'hi'
assert f'{x:^4}' == ' 10 '
#assert ('a' 'b' f'{x}' f'str<{y:^4}>' 'd' 'e') == 'ab10str< hi >de'

# Other tests
assert f'{{{4*10}}}' == '{40}'
