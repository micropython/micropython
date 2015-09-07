# lexer tests for things that are not implemented, or have non-compliant behaviour

# uPy requires spaces between literal numbers and keywords, CPy doesn't
try:
    eval('1and 0')
except SyntaxError:
    print('SyntaxError')
try:
    eval('1or 0')
except SyntaxError:
    print('SyntaxError')
try:
    eval('1if 1else 0')
except SyntaxError:
    print('SyntaxError')
try:
    eval('1if 0else 0')
except SyntaxError:
    print('SyntaxError')
