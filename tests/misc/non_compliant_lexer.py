# lexer tests for things that are not implemented, or have non-compliant behaviour


def test(code):
    try:
        exec(code)
        print("no Error")
    except SyntaxError:
        print("SyntaxError")
    except NotImplementedError:
        print("NotImplementedError")


# uPy requires spaces between literal numbers and keywords, CPy doesn't
try:
    eval("1and 0")
except SyntaxError:
    print("SyntaxError")
try:
    eval("1or 0")
except SyntaxError:
    print("SyntaxError")
try:
    eval("1if 1else 0")
except SyntaxError:
    print("SyntaxError")
try:
    eval("1if 0else 0")
except SyntaxError:
    print("SyntaxError")

# unicode name escapes are not implemented
test('"\\N{LATIN SMALL LETTER A}"')
