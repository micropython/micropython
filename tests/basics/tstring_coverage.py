# Comprehensive coverage tests for t-strings
# This file targets all edge cases and code paths for maximum coverage

from string.templatelib import Template, Interpolation

print("=== Constructor validation ===")
# Template.__new__ keyword argument error
try:
    Template(strings=("hello",))
except TypeError as e:
    print(f"Keyword arg error: {e}")

# Non-string/non-interpolation argument
try:
    Template("hello", 42, "world")
except TypeError as e:
    print(f"Invalid type error: {e}")

# Invalid tuple types when passed directly
try:
    Template(("hello",), (42,))
except TypeError as e:
    print(f"Non-Interpolation error: {e}")

# Mixed string/interpolation constructor
i1, i2, i3 = Interpolation(1, "1"), Interpolation(2, "2"), Interpolation(3, "3")
t_mixed = Template("start", i1, "middle", i2, "end")
print(f"Mixed constructor: strings={t_mixed.strings}")

# Empty template
t_empty = Template()
print(f"Empty template: strings={t_empty.strings}")

print("\n=== Format specifiers ===")
# ASCII conversion with unicode
unicode_str = "Café ☕"
i_ascii = Interpolation(unicode_str, "s", "a", "")
t_ascii = Template(("ASCII: ", ""), (i_ascii,))
print(f"ASCII conversion: {t_ascii.__str__()}")

# Format with conversion
pi = 3.14159
i_conv_fmt = Interpolation(pi, "pi", "r", ">10")
t_conv_fmt = Template(("Pi: ", ""), (i_conv_fmt,))
print(f"Conv+Format: '{t_conv_fmt.__str__()}'")

# Debug format
x = 42
i_debug = Interpolation(x, "x=", None, "")
t_debug = Template(("Debug: ", ""), (i_debug,))
print(f"Debug format: '{t_debug.__str__()}'")

# Format spec with interpolation
width = 10
i_fmt_interp = Interpolation(42, "value", None, "{width}")
t_fmt_interp = Template(("Value: ", ""), (i_fmt_interp,))
print(f"Format interpolation: '{t_fmt_interp.__str__()}'")

# Simple format spec
i_simple = Interpolation("test", "s", None, ">10")
t_simple = Template(("", ""), (i_simple,))
print(f"Simple format: '{t_simple.__str__()}'")

print("\n=== Binary operations ===")
# Template + str error
t = t"template"
try:
    result = t + "string"
except TypeError as e:
    print(f"Template + str: {e}")

# str + Template error
try:
    result = "string" + t
except TypeError as e:
    print(f"str + Template: {e}")

# Unsupported operations
try:
    result = t - t"other"
except TypeError:
    print("Subtract: unsupported")

print("\n=== Iterator edge cases ===")
# All empty strings
all_empty = Template(("", "", ""), (Interpolation(1, "a"), Interpolation(2, "b")))
items = [f"I({i.value})" if isinstance(i, Interpolation) else f"S('{i}')" for i in all_empty]
print(f"All empty iteration: {items}")

print("\n=== __template__ builtin ===")
# Invalid interpolation format
try:
    __template__(("test",), ((42,),))
except ValueError as e:
    print(f"Invalid format: {e}")

# Valid usage
strings = ("Hello ", "!")
interps = ((42, "x", None, ""),)
t_builtin = __template__(strings, interps)
print(f"__template__ result: {type(t_builtin).__name__}")

print("\n=== Large values property ===")
# More than 4 interpolations (forces heap allocation)
interps_large = [Interpolation(i, f"v{i}") for i in range(6)]
t_large = Template(tuple("" for _ in range(7)), tuple(interps_large))
vals = t_large.values
print(f"Large values: {len(vals)} items")

print("\n=== Size limits ===")
# Approaching template size limit
large_str = "x" * 30000
t_near_limit = Template((large_str, large_str, large_str), 
                       (Interpolation("a", "a"), Interpolation("b", "b")))
try:
    result = t_near_limit.__str__()
    print(f"Near limit: rendered {len(result)} chars")
except ValueError as e:
    print(f"Size error: {e}")

print("\n=== Parser tests ===")
# Expression length limit
try:
    long_expr = "x" * 10001
    exec(f't"{{{long_expr}}}"')
except ValueError as e:
    print(f"Length limit: {e}")
except SyntaxError:
    print("Length limit: SyntaxError")

# Empty expression
t_empty_expr = t"{}"
print(f"Empty expr: {t_empty_expr.interpolations[0].value}")

# Whitespace expression
t_ws = t"{   }"
print(f"Whitespace expr: {t_ws.interpolations[0].value}")

# Invalid syntax in expressions
invalid_exprs = [
    "{@}",  # Invalid token
    "{1 +* 2}",  # Invalid operator
    "{*x}",  # Invalid starred expr
]
for expr in invalid_exprs:
    try:
        exec(f't"{expr}"')
        print(f"ERROR: {expr} should fail")
    except SyntaxError:
        print(f"SyntaxError {expr}: OK")

# Complex nested expression
data = {"a": {"b": {"c": [1, 2, 3]}}}
t_nested = t"{data['a']['b']['c'][0]}"
print(f"Nested expr: {t_nested.__str__()}")

# Special constants
t_consts = t"{None}, {True}, {False}, {Ellipsis}"
print(f"Constants: {t_consts.__str__()}")

# Deeply nested structure (test node copying)
nested = 42
for _ in range(10):
    nested = [nested]
t_deep = t"{nested[0][0][0][0][0][0][0][0][0][0]}"
print(f"Deep nesting: {t_deep.__str__()}")

print("\n=== Complex concatenation ===")
# Many strings and interpolations
strings1 = tuple(f"s{i}" for i in range(5))
interps1 = tuple(Interpolation(i, f"v{i}") for i in range(4))
t1 = Template(strings1, interps1)

strings2 = tuple(f"t{i}" for i in range(3))
interps2 = tuple(Interpolation(i+10, f"w{i}") for i in range(2))
t2 = Template(strings2, interps2)

t_concat = t1 + t2
print(f"Complex concat: strings={len(t_concat.strings)}, interps={len(t_concat.interpolations)}")

print("\n=== Attribute access ===")
# __iter__ attribute
t = t"test"
iter_attr = getattr(t, "__iter__", None)
print(f"__iter__ attribute: {callable(iter_attr)}")

# Interpolation attributes and __match_args__
i = Interpolation(42, "x", "r", ".2f")
try:
    match_args = i.__match_args__
    print(f"__match_args__: {match_args}")
except AttributeError:
    print("__match_args__: not found")

# Read-only attributes
i2 = Interpolation(100, "value")
try:
    i2.value = 200
except AttributeError:
    print("value attribute: read-only")

print("\nCoverage tests completed!")