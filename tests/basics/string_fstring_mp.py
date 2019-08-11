# Tests the known limitations in the MicroPython implementation of f-strings.

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
print(f'h' f'i')
print(f'h' 'i')
print('h' f'i')
print(f'{x:^4}')
print('a' 'b' f'{x}' f'str<{y:^4}>' 'd' 'e')

## Raw f-strings are not supported in MicroPython
try:
    eval("rf'hello'")
except SyntaxError:
    print('SyntaxError')
else:
    print('Raw f-string (rf) did not raise SyntaxError')

try:
    eval("fr'hello'")
except SyntaxError:
    print('SyntaxError')
else:
    print('Raw f-string (fr) did not raise SyntaxError')
