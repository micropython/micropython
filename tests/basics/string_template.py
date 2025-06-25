# Test template strings (t-strings) - PEP 750

# Basic functionality tests
print("=== Basic t-string tests ===")

# Test 1: Basic template without interpolations
t1 = t"Hello World"
print(t1.strings)
print(len(t1.interpolations))

# Test 2: Empty t-string
t_empty = t""
print(t_empty.strings)
print(len(t_empty.interpolations))

# Test 3: Template with single interpolation
name = "MicroPython"
t2 = t"Hello {name}!"
print(t2.strings)
print(len(t2.interpolations))
print(t2.interpolations[0].value)
print(t2.interpolations[0].expression)
print(t2.values)

# Test 4: Multiple interpolations
x = 42
y = 3.14
t3 = t"x={x}, y={y}"
print(t3.strings)
print(len(t3.interpolations))
print(t3.values)

# Test 5: Complex expressions
items = [1, 2, 3]
t4 = t"Sum: {sum(items)}, Length: {len(items)}"
print(t4.values)

# Test 6: Nested brackets and slices
d = {"key": "value"}
t5 = t"Dict: {d['key']}, Slice: {items[1:]}"
print(t5.values)

# Test 7: Debug format {x=}
x = 42
t_debug = t"{x=}"
print(t_debug.interpolations[0].expression)
print(t_debug.interpolations[0].value)
print(t_debug.interpolations[0].conversion)

# Test 8: Format specifiers
value = 3.14159
t_fmt = t"Pi: {value:.2f}"
print(t_fmt.interpolations[0].format_spec)

# Test 9: Conversion specifiers
obj = "test"
t_conv = t"Repr: {obj!r}, Str: {obj!s}"
print([i.conversion for i in t_conv.interpolations])

# Test 10: Raw t-strings
t_raw = rt"Raw: \n\t"
print(repr(t_raw.strings[0]))

# Test 11: Adjacent interpolations
a = "A"
b = "B" 
t_adjacent = t"{a}{b}"
print(t_adjacent.strings)
print(len(t_adjacent.interpolations))

print("\n=== Template iteration tests ===")

# Test 12: Iteration
t_iter = t"a{x}b{y}c"
parts = list(t_iter)
print(len(parts))
print([type(p).__name__ for p in parts])
print([p if isinstance(p, str) else p.value for p in parts])

# Test 13: Empty strings omitted from iteration (PEP 750)
t_only_interp = t"{a}{b}"
parts2 = list(t_only_interp)
print(len(parts2))  # Should be 2 (only interpolations)

print("\n=== Advanced interpolation tests ===")

# Test 14: Function calls in interpolations
def greet(name):
    return f"Hello, {name}!"

t_func = t"Result: {greet('World')}"
print(t_func.interpolations[0].value)

# Test 15: Attribute access
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

p = Point(3, 4)
t_attr = t"Point: ({p.x}, {p.y})"
print([i.value for i in t_attr.interpolations])

# Test 16: Ternary operator
x = 10
t_ternary = t"Value is {'positive' if x > 0 else 'negative'}"
print(t_ternary.interpolations[0].value)

# Test 17: Template type checks
print(f"\nType checks: Template={type(t1).__name__}, is Template={hasattr(t1, 'strings')}")

# Test 18: Values property
t_vals = t"Values: {1}, {2}, {3}"
print(t_vals.values)

# Test 19: No interpolations case
t_no_interp = t"Just a string"
print(len(t_no_interp.interpolations))
print(t_no_interp.values)

print("\n=== Format spec interpolation tests ===")

# Test 20: Basic format_spec interpolation
precision = 2
value = 3.14159
t_fmt_interp = t"Pi: {value:.{precision}f}"
print(f"Format spec stored: '{t_fmt_interp.interpolations[0].format_spec}'")
print(f"Result: {t_fmt_interp.__str__()}")

# Test 21: Multiple interpolations in format_spec  
width = 10
align = ">"
t_fmt_multi = t"Aligned: {42:{align}{width}d}"
print(f"Format spec: '{t_fmt_multi.interpolations[0].format_spec}'")
print(f"Result: '{t_fmt_multi.__str__()}'")

# Test 22: Complex format spec
fill_char = "*"
precision = 3
t_fmt_complex = t"Complex: {99.9:{fill_char}>{width}.{precision}f}"
print(f"Complex format spec: '{t_fmt_complex.interpolations[0].format_spec}'")

# Test 23: Format spec with escaped braces
t_fmt_escaped = t"Escaped: {42:{{width}}}"
print(f"Escaped braces format spec: '{t_fmt_escaped.interpolations[0].format_spec}'")

print("\n=== Template concatenation tests ===")

# Test 24: Template + Template concatenation
t_concat1 = t"Hello "
t_concat2 = t"{42}"
t_concat3 = t" world"
try:
    result = t_concat1 + t_concat2
    print(f"Template + Template: type={type(result).__name__}")
    print(f"Concatenated strings: {result.strings}")
    print(f"Concatenated values: {result.values}")
except TypeError as e:
    print(f"Template concatenation error: {e}")

# Test 25: Chained concatenation
try:
    result = t_concat1 + t_concat2 + t_concat3
    print(f"Chained concatenation strings: {result.strings}")
except TypeError as e:
    print(f"Chained concatenation error: {e}")

# Test 26: Template + str (should fail)
try:
    result = t_concat1 + "regular string"
    print(f"ERROR: Template + str should raise TypeError")
except TypeError as e:
    print(f"Template + str correctly raises: {e}")

# Test 27: str + Template (should fail)
try:
    result = "Start: " + t_concat2
    print(f"ERROR: str + Template should raise TypeError")
except TypeError as e:
    print(f"str + Template correctly raises: {e}")

print("\n=== Additional edge cases ===")

# Test 28: ASCII conversion (!a)
obj_unicode = "Hello\\nWorld\\u00A9"  # Contains newline and copyright symbol
t_ascii = t"ASCII: {obj_unicode!a}"
print(f"ASCII conversion: {t_ascii.interpolations[0].conversion}")

# Test 29: Template with only interpolations
a = 1
b = 2
t_only_interp = t"{a}{b}"
print(f"Interpolation-only template strings: {t_only_interp.strings}")
print(f"Interpolation-only template values: {t_only_interp.values}")

# Test 30: __str__ method behavior
t_str_test = t"Test {42}"
print(f"__str__ result: {t_str_test.__str__()}")

# Test 31: Template constructor validation (via __template__)
try:
    # This would be called internally by t-string literals
    result = __template__(("Hello", "!"), ((42, "42", None, ""),))
    print(f"__template__ result type: {type(result).__name__}")
except Exception as e:
    print(f"__template__ error: {e}")