# Basic t-string tests covering core functionality and parser behavior
from string.templatelib import Template, Interpolation

print("=== Basic functionality ===")
# Basic template string creation
t = t"Hello World"
print(type(t).__name__)

# Template with interpolation
name = "World"
t2 = t"Hello {name}"
print(f"Strings: {t2.strings}")
print(f"Value: {t2.interpolations[0].value}")

# Template string conversion
print(f"str(): {t2.__str__()}")

# Raw t-strings
t_raw = rt"Path: C:\test\{name}"
print(f"Raw: '{t_raw.strings[0]}'")

# Reverse raw
t_tr = tr"Path: C:\test\{name}"
print(f"tr: '{t_tr.strings[0]}'")

print("\n=== Parser tests ===")
# Empty expression
t_empty = t"{}"
print(f"Empty expr: {t_empty.interpolations[0].value}")

# Whitespace handling
t_ws = t"{ 42 }"
print(f"Whitespace: {t_ws.__str__()}")

# Complex expressions
data = {"a": {"b": [1, 2, 3]}}
t_complex = t"{data['a']['b'][0]}"
print(f"Complex: {t_complex.__str__()}")

# Constants
print(f"None: {t'{None}'.__str__()}")
print(f"True: {t'{True}'.__str__()}")
print(f"False: {t'{False}'.__str__()}")
print(f"Ellipsis: {t'{...}'.__str__()}")

# Nested expressions
obj = type('Obj', (), {
    '__getattr__': lambda s, n: s,
    '__getitem__': lambda s, k: s,
    '__call__': lambda s, *a: 42,
    '__str__': lambda s: "42"
})()
print(f"Deep nest: {t'{obj.a.b[0].c()}'.__str__()}")

print("\n=== Conversions and formatting ===")
# Conversion specifiers
val = {"key": "value"}
print(f"repr: {t'{val!r}'.__str__()}")
print(f"str: {t'{val!s}'.__str__()}")
print(f"ascii: {t'{val!a}'.__str__()}")

# Format specifications
print(f"Width: '{t'{42:10d}'.__str__()}'")
print(f"Precision: {t'{3.14159:.2f}'.__str__()}")

# Debug format
x = 42
print(f"Debug: {t'{x=}'.__str__()}")

print("\n=== Constructor tests ===")
# Empty constructor
t_empty = Template()
print(f"Empty: {t_empty.strings}")

# Single string
t_single = Template("single")
print(f"Single: {t_single.strings}")

# Multiple strings
t_multi = Template("a", "b", "c")
print(f"Multi: {t_multi.strings}")

# Mixed with interpolations
i1 = Interpolation(1, "x")
i2 = Interpolation(2, "y")
t_mixed = Template("start", i1, "middle", i2, "end")
print(f"Mixed: strings={t_mixed.strings}, values={t_mixed.values}")

print("\n=== Operations ===")
# Concatenation
t1 = t"Hello"
t2 = t" World"
t_concat = t1 + t2
print(f"Concat: '{t_concat.__str__()}'")

# Iterator
items = list(t"a{1}b{2}c")
print(f"Iterator: {[type(x).__name__ for x in items]}")

# Attributes
t_attr = t"test{42}"
print(f"strings attr: {t_attr.strings}")
print(f"interpolations attr: {len(t_attr.interpolations)}")
print(f"values attr: {t_attr.values}")

# repr
print(f"repr: {repr(t2)[:50]}...")

print("\n=== Error cases ===")
# Invalid syntax
try:
    exec('t"{@}"')
except SyntaxError:
    print("Invalid syntax: SyntaxError")

# Expression too long
try:
    long_expr = "x" * 10001
    exec(f't"{{{long_expr}}}"')
except (ValueError, SyntaxError):
    print("Long expr: Error")

# Invalid concatenation
try:
    exec('t"hello" "world"')
except SyntaxError:
    print("Mixed concat: SyntaxError")

# Invalid prefix
try:
    exec('bt"test"')
except SyntaxError:
    print("bt prefix: SyntaxError")

print("\nBasic tests completed!")