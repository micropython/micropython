# Core functionality tests for PEP 750 template strings (t-strings)
# This file combines basic functionality, attributes, and rendering tests

# === Basic template string creation and type ===
t = t"Hello World"
print(type(t).__name__)

# Template with interpolation
name = "World"
t2 = t"Hello {name}"
print(f"Strings: {t2.strings}")
print(f"Interpolations count: {len(t2.interpolations)}")

# Accessing interpolation details
interp = t2.interpolations[0]
print(f"Value: {interp.value}")
print(f"Expression: {interp.expression}")
print(f"Conversion: {interp.conversion}")
print(f"Format spec: '{interp.format_spec}'")

# Values property
x = 10
y = 20
t3 = t"x={x}, y={y}"
print(f"Values: {t3.values}")

# === Format specifiers and conversions ===
value = 42.12345
t4 = t"Value: {value:.2f}"
print(f"Format spec: '{t4.interpolations[0].format_spec}'")

# Conversion specifiers
obj = "test"
t5 = t"{obj!r} {obj!s} {obj!a}"
print(f"Conversions: {[i.conversion for i in t5.interpolations]}")

# Format spec with conversion
t22 = t"{value!r:.2f}"
print(f"Conv+format: conv={t22.interpolations[0].conversion}, fmt='{t22.interpolations[0].format_spec}'")

# === Template concatenation ===
t7 = t"Hello " + t"{name}"
print(f"Concat result type: {type(t7).__name__}")
print(f"Concat strings: {t7.strings}")

# String + Template concatenation should fail
try:
    t8 = "Prefix: " + t"{value}"
    print("ERROR: String+Template should fail")
except TypeError:
    print("String+Template rejected: OK")

# === Iterator protocol ===
t18 = t"a{1}b{2}c"
parts = list(t18)
print(f"Iterator parts: {[type(p).__name__ for p in parts]}")

# === __str__ method and rendering ===
t19 = t"test"
print(f"Has __str__: {hasattr(t19, '__str__')}")

# Test actual rendering
name = "Alice"
t_render = t"Hello {name}!"
result = t_render.__str__()
print(f"Rendered: '{result}'")

# Multiple interpolations rendering
a, b, c = 10, 20, 30
t_multi = t"{a} + {b} = {c}"
result = t_multi.__str__()
print(f"Multiple render: '{result}'")

# === Interpolation attributes and __match_args__ ===
from string.templatelib import Template, Interpolation

# Create an interpolation
i = Interpolation(42, "x", "r", ".2f")

# Test __match_args__ attribute
try:
    match_args = i.__match_args__
    print(f"__match_args__: {match_args}")
except AttributeError as e:
    print(f"ERROR: {e}")

# Test that Interpolation attributes are read-only
i2 = Interpolation(100, "value")
try:
    i2.value = 200
    print("ERROR: Should not be able to set value")
except AttributeError as e:
    print(f"value read-only: OK")

# === Debug format ===
var = 42
# Create interpolation with expression ending in '='
i_debug = Interpolation(var, "var=", None, "")
t_debug = Template(("Debug: ", ""), (i_debug,))
result = t_debug.__str__()
print(f"Debug format: '{result}'")

# === Template representation ===
print(f"Template repr: {repr(t2)[:30]}...")

# === Special cases ===
# Empty template
t10 = t""
print(f"Empty: strings={t10.strings}, interps={t10.interpolations}")

# Template with all empty strings between interpolations
t20 = t"{a}{b}"
print(f"Adjacent interps: strings={t20.strings}")

# Unicode in template strings
emoji = "🎉"
t21 = t"Celebration {emoji}"
print(f"Unicode value: {t21.interpolations[0].value}")

# Raw template strings
t15 = rt"Path: C:\Users\{name}"
print(f"Raw string: '{t15.strings[0]}'")

# Implicit concatenation
t16 = t"Part1 " t"Part2"
print(f"Implicit concat: '{t16.strings[0]}'")

print("Core tests completed!")