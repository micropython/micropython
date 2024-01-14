try:
    from codeop import compile_command
except ImportError:
    print("SKIP")
    raise SystemExit


def test(snippet):
    result = compile_command(snippet)
    print("None" if result is None else "<code>")


# Complete command
test("3+3")

# Complete command
test("if 1:\n    print('hi mom')\n")

# Incomplete command
test("if 1:")

# Incomplete multiline string
test("'''I'm sure it's OK")

# Incomplete parenthesized expression
test("[1, 2")
