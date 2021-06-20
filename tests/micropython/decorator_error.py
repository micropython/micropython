# test syntax errors for uPy-specific decorators


def test_syntax(code):
    try:
        exec(code)
    except SyntaxError:
        print("SyntaxError")


# invalid micropython decorators
test_syntax("@micropython.a\ndef f(): pass")
test_syntax("@micropython.a.b\ndef f(): pass")
