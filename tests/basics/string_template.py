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