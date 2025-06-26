# Edge case tests for t-strings
# This file contains tests for hard-to-reach code paths and corner cases

from string.templatelib import Template, Interpolation

print("=== Conversion specifiers edge cases ===")

# Test all conversion types with rendering
obj = "Test String"

# repr conversion
i_repr = Interpolation(obj, "obj", "r", "")
t_repr = Template(("Result: ", ""), (i_repr,))
print(f"repr: {t_repr.__str__()}")

# str conversion  
i_str = Interpolation(obj, "obj", "s", "")
t_str = Template(("Result: ", ""), (i_str,))
print(f"str: {t_str.__str__()}")

# ascii conversion
obj_unicode = "Hello 🌍"
i_ascii = Interpolation(obj_unicode, "obj", "a", "")
t_ascii = Template(("Result: ", ""), (i_ascii,))
print(f"ascii: {t_ascii.__str__()}")

# Conversion with format spec
pi = 3.14159
i_conv_fmt = Interpolation(pi, "pi", "r", ">10")
t_conv_fmt = Template(("Pi: ", ""), (i_conv_fmt,))
result = t_conv_fmt.__str__()
print(f"Conv+Format: '{result}'")

print("\n=== Debug format edge cases ===")

# Create interpolation with expression ending in '=' to trigger debug format
x = 42
i_debug = Interpolation(x, "x=", None, "")
t_debug = Template(("Debug: ", ""), (i_debug,))
result = t_debug.__str__()
print(f"Debug format: '{result}'")

# Complex expression with '='
vals = [1, 2, 3]
i_debug2 = Interpolation(3, "vals[0]+vals[1]=", None, "")
t_debug2 = Template(("Result: ", ""), (i_debug2,))
result2 = t_debug2.__str__()
print(f"Complex debug: '{result2}'")

print("\n=== Format spec edge cases ===")

# Format spec with interpolation
width = 10
i_fmt_interp = Interpolation(42, "value", None, "{width}")
t_fmt_interp = Template(("Value: ", ""), (i_fmt_interp,))
result = t_fmt_interp.__str__()
print(f"Format interpolation: '{result}'")

# Format spec with escaped double braces }}
# This tests a specific code path in format spec parsing
i_double = Interpolation("test", "s", None, "text}}")
t_double = Template(("", ""), (i_double,))
try:
    result = t_double.__str__()
    print(f"Double }}: '{result}'")
except ValueError as e:
    print(f"Double }} error: {e}")

# Format spec with text after interpolation
width = 5
i_text = Interpolation("abc", "s", None, "{width}more")
t_text = Template(("", ""), (i_text,))
try:
    result = t_text.__str__()
    print(f"Text after interp: '{result}'")
except ValueError as e:
    print(f"Text after interp error: {e}")

print("\n=== Binary operation edge cases ===")

# Test reverse add with string (different code path)
t = t"template"
try:
    result = "string" + t
    print(f"ERROR: str + Template should fail")
except TypeError:
    print("str + Template: TypeError OK")

# Test binary op returning MP_OBJ_NULL
try:
    result = t - t"other"
    print(f"ERROR: Template - Template should fail")
except TypeError:
    print("Template - Template: TypeError OK")

print("\n=== Iterator edge cases ===")

# Template with adjacent interpolations (triggers at_interpolation logic)
t_adjacent = Template(("", "", "", ""), (Interpolation(1, "a"), Interpolation(2, "b"), Interpolation(3, "c")))
items = []
for item in t_adjacent:
    if isinstance(item, Interpolation):
        items.append(f"I({item.value})")
    else:
        items.append(f"S('{item}')")
print(f"Adjacent iteration: {items}")

print("\n=== Error path edge cases ===")

# Test __template__ with invalid interpolation format
try:
    __template__(("test",), ((42,),))  # Only 1 item
except ValueError as e:
    print(f"Invalid interpolation: {e}")

# Test template size limit during rendering
try:
    huge_str = "x" * 210000
    strings = (huge_str, huge_str, huge_str, huge_str, huge_str, huge_str)
    interps = tuple(Interpolation(f"val{i}", f"v{i}") for i in range(5))
    t_huge = Template(strings, interps)
    result = t_huge.__str__()
    print(f"ERROR: Should have failed with size limit")
except (ValueError, MemoryError) as e:
    print(f"Size limit: {type(e).__name__}")

print("\n=== Attribute access edge cases ===")

# Test __iter__ attribute access
t = t"test"
iter_attr = getattr(t, "__iter__", None)
if iter_attr:
    print(f"__iter__ attribute: {callable(iter_attr)}")

# Test large values property (>4 items for heap allocation)
interps_large = [Interpolation(i, f"v{i}") for i in range(6)]
t_large = Template(tuple("" for _ in range(7)), tuple(interps_large))
vals = t_large.values
print(f"Large values: {len(vals)} items")

print("\nEdge case tests completed!")