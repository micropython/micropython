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

# PEP-0498 specifies that '\\' and '#' must be disallowed explicitly, whereas
# MicroPython relies on the syntax error as a result of the substitution.

print(f"\\")
print(f'#')
try:
    eval("f'{\}'")
except SyntaxError:
    print('SyntaxError')
try:
    eval("f'{#}'")
except SyntaxError:
    print('SyntaxError')


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
