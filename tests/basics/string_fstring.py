def f():
    return 4
def g(_):
    return 5
def h():
    return 6

print(f'no interpolation')
print(f"no interpolation")
print(f"""no interpolation""")

x, y = 1, 2
print(f'{x}')
print(f'{x:08x}')
print(f'a {x} b {y} c')
print(f'a {x:08x} b {y} c')

print(f'a {"hello"} b')
print(f'a {f() + g("foo") + h()} b')

def foo(a, b):
    return f'{x}{y}{a}{b}'
print(foo(7, 8))

# ':' character within {...} that should not be interpreted as format specifiers.
print(f"a{[0,1,2][0:2]}")
print(f"a{[0,15,2][0:2][-1]:04x}")

# Nested '{' and '}' characters.
print(f"a{ {0,1,2}}")

# PEP-0498 specifies that handling of double braces '{{' or '}}' should
# behave like str.format.
print(f'{{}}')
print(f'{{{4*10}}}', '{40}')

# A single closing brace, unlike str.format should raise a syntax error.
# MicroPython instead raises ValueError at runtime from the substitution.
try:
    eval("f'{{}'")
except (ValueError, SyntaxError):
    # MicroPython incorrectly raises ValueError here.
    print('SyntaxError')

# Allow literal tuples
print(f"a {1,} b")
print(f"a {x,y,} b")
print(f"a {x,1} b")

# f-strings with conversion specifiers (only support !r and !s).
a = "123"
print(f"{a!r}")
print(f"{a!s}")
try:
    eval('print(f"{a!x}")')
except (ValueError, SyntaxError):
    # CPython detects this at compile time, MicroPython fails with ValueError
    # when the str.format is executed.
    print("ValueError")

# Mixing conversion specifiers with formatting.
print(f"{a!r:8s}")
print(f"{a!s:8s}")

# Still allow ! in expressions.
print(f"{'1' if a != '456' else '0'!r:8s}")
print(f"{'1' if a != '456' else '0'!s:8s}")

# Concatenation of adjacent f-strings.
print(f"" f"")
print(f"a" f"b")
print(f"{x}" f"{y}")
print(
    f"a{x}b---------------------------------"
    f"cd---------------------------------"
    f"e{y}f---------------------------------"
)

# Raw f-strings.
print(rf"\r\a\w {'f'} \s\t\r\i\n\g")
print(fr"\r{x}")
