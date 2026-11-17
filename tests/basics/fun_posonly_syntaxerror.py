# test syntax errors for positional-only arguments (PEP 570)


def test_syntax(code):
    try:
        exec(code)
        print("no SyntaxError")
    except SyntaxError:
        print("SyntaxError")


# slash with no args before it
test_syntax("def f(/): pass")
test_syntax("def f(/, a): pass")

# more than one slash
test_syntax("def f(a, /, /): pass")

# slash after a star or double-star
test_syntax("def f(*, /, a): pass")
test_syntax("def f(*args, /): pass")
test_syntax("def f(**kwargs, /): pass")

# non-default arg following default arg
test_syntax("def f(a=1, b, /): pass")
test_syntax("def f(a=1, /, b): pass")

# duplicate argument names involving positional-only args
test_syntax("def f(a, /, a): pass")

# slash can't have a default or an annotation
test_syntax("def f(a, / = 1): pass")
test_syntax("def f(a, /: int): pass")

# lambdas
test_syntax("lambda /: None")
test_syntax("lambda a, /, /: None")
