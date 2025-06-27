# Basic functionality and syntax tests for PEP 750 template strings (t-strings)
# This file combines core functionality, syntax, and import tests

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

# === Import location tests ===
# Templates should NOT be in builtins
try:
    Template
    print("ERROR: Template should not be a builtin")
except NameError:
    print("Template not in builtins: OK")

# Correct import location
from string.templatelib import Template, Interpolation
print("Imported Template and Interpolation from string.templatelib")

# Verify types
print(f"Template type name: {Template.__name__}")
print(f"Interpolation type name: {Interpolation.__name__}")

# Test that t-string literals create the same types
name = "World"
t_literal = t"Hello {name}"
print(f"Is same Template type: {type(t_literal) is Template}")
print(f"Is same Interpolation type: {type(t_literal.interpolations[0]) is Interpolation}")

# === Syntax errors ===
# Invalid prefix combinations
try:
    exec('bt"test"')
    print("ERROR: bt prefix should fail")
except SyntaxError:
    print("bt prefix: SyntaxError")

try:
    exec('ft"test"')
    print("ERROR: ft prefix should fail") 
except SyntaxError:
    print("ft prefix: SyntaxError")

# Cannot mix t-string and regular string
try:
    exec('t"hello" "world"')
    print("ERROR: Should not allow t-string + string literal")
except SyntaxError:
    print("Cannot mix t-string and regular string")

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

# === Complex expressions ===
# Nested expressions
lst = [1, 2, 3]
t11 = t"Sum: {lst[0] + lst[1] + lst[2]}"
print(f"Nested expr value: {t11.interpolations[0].value}")

# Method calls in expressions
t12 = t"Upper: {'hello'.upper()}"
print(f"Method call: {t12.interpolations[0].value}")

# Lambda expressions
t13 = t"Lambda: {(lambda x: x*2)}"
print(f"Lambda type: {type(t13.interpolations[0].value).__name__}")

# === Debug format ===
var = 42
# Create interpolation with expression ending in '='
i_debug = Interpolation(var, "var=", None, "")
t_debug = Template(("Debug: ", ""), (i_debug,))
result = t_debug.__str__()
print(f"Debug format: '{result}'")

# === Template representation ===
print(f"Template repr: {repr(t2)[:30]}...")

# === Type validation ===
# isinstance checks
print(f"isinstance(t_literal, Template): {isinstance(t_literal, Template)}")
i = Interpolation(42, "x")
print(f"isinstance(i, Interpolation): {isinstance(i, Interpolation)}")

print("\nBasic tests completed!")