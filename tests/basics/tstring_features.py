# Advanced features and edge cases for t-strings
# This file combines advanced, extreme, and parser tests

from string.templatelib import Template, Interpolation

print("=== Complex expressions ===")

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

# Complex expressions
t_ternary = t"Result: {1 if True else 2}"
print(f"Ternary: {t_ternary.__str__()}")

t_listcomp = t"List: {[x for x in range(3)]}"
print(f"List comp: {t_listcomp.__str__()}")

print("\n=== Format spec interpolation ===")

# Complex format with nested interpolations
width = 10
precision = 2
value = 123.456
t14 = t"{value:{width}.{precision}f}"
result = t14.__str__()
print(f"Interpolated format: '{result}'")

# Format spec with escaped braces
i_escaped = Interpolation(42, "value", None, "<10")
t_escaped = Template(("Value: ", ""), (i_escaped,))
result = t_escaped.__str__()
print(f"Left aligned: '{result}'")

print("\n=== Type validation and constructors ===")

# Test template constructor validation
try:
    Template("not a tuple", ())
except TypeError as e:
    print(f"strings validation: {e}")

try:
    Template((), "not a tuple")
except TypeError as e:
    print(f"interpolations validation: {e}")

try:
    Template((1, 2, 3), ())
except TypeError as e:
    print(f"strings element validation: {e}")

try:
    Template(("a", "b"), ("not an interpolation",))
except TypeError as e:
    print(f"interpolations element validation: {e}")

print("\n=== Memory and performance tests ===")

# Many interpolations
vals = list(range(10))
interps = tuple(Interpolation(v, f"vals[{i}]") for i, v in enumerate(vals))
strings = tuple("" for _ in range(11))
t_many = Template(strings, interps)
print(f"Many interpolations: count={len(t_many.interpolations)}, values={t_many.values}")

# Large template (but within limits)
try:
    big_str = "x" * 50000  # 50KB
    strings = (big_str,) * 20  # 1MB total
    interps = tuple(Interpolation(i, f"v{i}") for i in range(19))
    t_big = Template(strings, interps)
    print(f"Large template created: strings={len(t_big.strings)}")
except MemoryError:
    print("Large template: MemoryError")

print("\n=== Iterator edge cases ===")

# Only interpolations (empty strings)
t_only_interp = Template(("", "", "", ""), (Interpolation(1, "a"), Interpolation(2, "b"), Interpolation(3, "c")))
items = [type(x).__name__ for x in t_only_interp]
print(f"Only interp: {items}")

# Mixed empty and non-empty strings
t_mixed = Template(("a", "", "b", "", "c"), (Interpolation(1, "x"), Interpolation(2, "y")))
items = [str(x) if isinstance(x, str) else f"I({x.value})" for x in t_mixed]
print(f"Mixed iteration: {items}")

print("\n=== Concatenation edge cases ===")

# Concatenate with non-Template types
t = t"template"
try:
    result = t + 42
except TypeError:
    print("OK: Cannot concatenate Template with int")

try:
    result = 42 + t
except TypeError:
    print("OK: Cannot concatenate int with Template")

# Complex concatenation
strings1 = tuple(f"s{i}" for i in range(5))
interps1 = tuple(Interpolation(i, f"v{i}") for i in range(4))
t1 = Template(strings1, interps1)

strings2 = tuple(f"t{i}" for i in range(3))
interps2 = tuple(Interpolation(i+10, f"w{i}") for i in range(2))
t2 = Template(strings2, interps2)

t_concat = t1 + t2
print(f"Complex concat: strings={len(t_concat.strings)}, interps={len(t_concat.interpolations)}")

print("\n=== Expression parser tests ===")

# Empty/whitespace expressions
for expr_str in ["", " ", "  ", "\t", "\n"]:
    t_empty = t"{}"  # Will use empty expression
    if t_empty.interpolations:
        print(f"Empty expr: '{t_empty.interpolations[0].expression}' -> {t_empty.interpolations[0].value}")

# Special literals
t_none = t"{None}"
print(f"None literal: {t_none.__str__()}")

t_str = t"{'string literal'}"
print(f"String literal: {t_str.__str__()}")

t_ellipsis = t"{...}"
print(f"Ellipsis: {t_ellipsis.__str__()}")

print("\n=== Size and error handling ===")

# Test approaching template size limit
try:
    # This should trigger the size limit error when rendering
    huge_str = "x" * 210000
    strings = (huge_str, huge_str, huge_str, huge_str, huge_str, huge_str)
    interps = tuple(Interpolation(f"val{i}", f"v{i}") for i in range(5))
    t_huge = Template(strings, interps)
    result = t_huge.__str__()
    print(f"ERROR: Should have failed with size limit")
except ValueError as e:
    print(f"Size limit error: {e}")
except MemoryError:
    print("Size limit error: MemoryError")

# Test __template__ with invalid interpolation format
try:
    __template__(("test",), ((42,),))  # Only 1 item in interpolation tuple
    print("ERROR: Should have failed")
except ValueError as e:
    print(f"Invalid interpolation format: {e}")

# Test values property with large array
interps_large = [Interpolation(i, f"v{i}") for i in range(6)]  # More than 4 to force heap allocation
t_large = Template(tuple("" for _ in range(7)), tuple(interps_large))
vals = t_large.values
print(f"Large values array: {len(vals)} items")

print("\nFeatures tests completed!")