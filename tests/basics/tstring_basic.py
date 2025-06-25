# Test the core functionality of PEP 750 template strings (t-strings)

# Basic template string creation and type
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

# Format specifiers
value = 42.12345
t4 = t"Value: {value:.2f}"
print(f"Format spec: '{t4.interpolations[0].format_spec}'")

# Conversion specifiers
obj = "test"
t5 = t"{obj!r} {obj!s} {obj!a}"
print(f"Conversions: {[i.conversion for i in t5.interpolations]}")

# Debug format (=)
var = 42
t6 = t"{var=}"
print(f"Debug format: '{t6.strings[0]}', value={t6.interpolations[0].value}")

# Template concatenation
t7 = t"Hello " + t"{name}"
print(f"Concat result type: {type(t7).__name__}")
print(f"Concat strings: {t7.strings}")

# String + Template concatenation
t8 = "Prefix: " + t"{value}"
print(f"String+Template type: {type(t8).__name__}")

# Multiple interpolations
a, b, c = 1, 2, 3
t9 = t"{a}{b}{c}"
print(f"Multiple: strings={len(t9.strings)}, interps={len(t9.interpolations)}")

# Empty template
t10 = t""
print(f"Empty: strings={t10.strings}, interps={t10.interpolations}")

# Nested expressions
lst = [1, 2, 3]
t11 = t"Sum: {lst[0] + lst[1] + lst[2]}"
print(f"Nested expr value: {t11.interpolations[0].value}")

# Method calls in expressions
t12 = t"Upper: {'hello'.upper()}"
print(f"Method call: {t12.interpolations[0].value}")

# Lambda expressions (values are not evaluated until accessed)
t13 = t"Lambda: {(lambda x: x*2)}"
print(f"Lambda type: {type(t13.interpolations[0].value).__name__}")

# Complex format with nested interpolations
width = 10
precision = 2
t14 = t"{value:{width}.{precision}f}"
print(f"Nested format result: '{t14.interpolations[0].format_spec}'")

# Raw template strings
t15 = rt"Path: C:\Users\{name}"
print(f"Raw string: '{t15.strings[0]}'")

# Implicit concatenation
t16 = t"Part1 " t"Part2"
print(f"Implicit concat: '{t16.strings[0]}'")

# Mixed implicit concatenation (string literal + t-string)
# Note: This creates a Template, not a string
t17 = ("Start " t"End")
print(f"Mixed implicit type: {type(t17).__name__}")

# Iterator protocol
t18 = t"a{1}b{2}c"
parts = list(t18)
print(f"Iterator parts: {[type(p).__name__ for p in parts]}")

# __str__ method access
t19 = t"test"
print(f"Has __str__: {hasattr(t19, '__str__')}")

# Verify __str__ is callable
result = t19.__str__()
print(f"__str__ result type: {type(result).__name__}")

# Template with all empty strings between interpolations
t20 = t"{a}{b}"
print(f"Adjacent interps: strings={t20.strings}")

# Unicode in template strings
emoji = "🎉"
t21 = t"Celebration {emoji}"
print(f"Unicode value: {t21.interpolations[0].value}")

# Format spec with conversion
t22 = t"{value!r:.2f}"
print(f"Conv+format: conv={t22.interpolations[0].conversion}, fmt='{t22.interpolations[0].format_spec}'")

# Template representation
print(f"Template repr: {repr(t2)[:30]}...")

# Accessing template internals
print(f"t2 type check: {type(t2).__name__ == 'Template'}")

# Expression scope test - accessing locals and globals
global_var = "global"
def test_scope():
    local_var = "local"
    t = t"{local_var} and {global_var}"
    return t.values

print(f"Scope test: {test_scope()}")

print("Basic tests completed!")